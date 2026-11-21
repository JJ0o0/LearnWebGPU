#include <j0o0ll/core/device.hpp>

#include <j0o0ll/diagnostics/error.hpp>

#include <j0o0ll/utils/async_utils.hpp>
#include <j0o0ll/utils/string_utils.hpp>

#include <webgpu/webgpu.h>

#include <utility>
#include <string>

namespace j0o0ll {
    Device::Device(WGPUDevice device) 
        : m_device(device) {}

    Device::~Device() {
        if (m_device) {
            wgpuDeviceRelease(m_device);
        }
    }

    Device::Device(Device&& other) noexcept 
        : m_device(std::exchange(other.m_device, nullptr)) {}
    
    Device& Device::operator=(Device&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        if (m_device) {
            wgpuDeviceRelease(m_device);
        }

        m_device = std::exchange(other.m_device, nullptr);
        return *this;
    }

    WGPUDevice Device::get() const noexcept {
        return m_device;
    }

    Result<Device> Device::request(WGPUInstance instance, WGPUAdapter adapter, const WGPUDeviceDescriptor* descriptor) {
        struct UserData {
            WGPUDevice device = nullptr;
            bool request_ended = false;

            std::string error_message;
        };

        UserData user_data;

        auto on_device_request_ended = [](
            WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message,
            void* user_data1, void*
        ) {
            UserData& user_data = *reinterpret_cast<UserData*>(user_data1);
            if (status == WGPURequestDeviceStatus_Success) {
                user_data.device = device;
            } else {
                user_data.error_message = to_std_string_view(message);
            }

            user_data.request_ended = true;
        };

        WGPURequestDeviceCallbackInfo callback_info = {
            nullptr,
            WGPUCallbackMode_AllowProcessEvents,
            on_device_request_ended,
            &user_data,
            nullptr  
        };

        wgpuAdapterRequestDevice(adapter, descriptor, callback_info);

        wgpuInstanceProcessEvents(instance);
        while (!user_data.request_ended) {
            sleep_for_milliseconds(200);
            wgpuInstanceProcessEvents(instance);
        }

        if (!user_data.device) {
            return Error(
                ErrorCode::DeviceRequestFailed,
                user_data.error_message
            );
        }

        return Device(user_data.device);
    }
}