#include <j0o0ll/core/adapter.hpp>

#include <j0o0ll/diagnostics/error.hpp>

#include <j0o0ll/utils/async_utils.hpp>
#include <j0o0ll/utils/string_utils.hpp>

#include <webgpu/webgpu.h>

#include <utility>
#include <string>

namespace j0o0ll {
    Adapter::Adapter(WGPUAdapter adapter) 
        : m_adapter(adapter) {}

    Adapter::~Adapter() {
        if (m_adapter) {
            wgpuAdapterRelease(m_adapter);
        }
    }

    Adapter::Adapter(Adapter&& other) noexcept 
        : m_adapter(std::exchange(other.m_adapter, nullptr)) {}
    
    Adapter& Adapter::operator=(Adapter&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        if (m_adapter) {
            wgpuAdapterRelease(m_adapter);
        }

        m_adapter = std::exchange(other.m_adapter, nullptr);
        return *this;
    }

    WGPUAdapter Adapter::get() const noexcept {
        return m_adapter;
    }

    Result<Adapter> Adapter::request(WGPUInstance instance, const WGPURequestAdapterOptions* options) {
        struct UserData {
            WGPUAdapter adapter = nullptr;
            bool request_ended = false;

            std::string error_message;
        };

        UserData user_data;

        auto on_adapter_request_ended = [](
            WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message,
            void* user_data1, void*
        ) {
            UserData& user_data = *reinterpret_cast<UserData*>(user_data1);
            if (status == WGPURequestAdapterStatus_Success) {
                user_data.adapter = adapter;
            } else {
                user_data.error_message = to_std_string_view(message);
            }

            user_data.request_ended = true;
        };

        WGPURequestAdapterCallbackInfo callback_info = {
            nullptr,
            WGPUCallbackMode_AllowProcessEvents,
            on_adapter_request_ended,
            &user_data,
            nullptr  
        };

        wgpuInstanceRequestAdapter(instance, options, callback_info);

        wgpuInstanceProcessEvents(instance);
        while (!user_data.request_ended) {
            sleep_for_milliseconds(200);
            wgpuInstanceProcessEvents(instance);
        }

        if (!user_data.adapter) {
            return Error(
                ErrorCode::AdapterRequestFailed,
                user_data.error_message
            );
        }

        return Adapter(user_data.adapter);
    }
}