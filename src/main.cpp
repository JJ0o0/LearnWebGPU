#include <j0o0ll/diagnostics/detail/emit.hpp>
#include <j0o0ll/diagnostics/inspect.hpp>
#include <j0o0ll/diagnostics/log.hpp>

#include <j0o0ll/utils/string_utils.hpp>

#include <j0o0ll/core/instance.hpp>
#include <j0o0ll/core/adapter.hpp>
#include <j0o0ll/core/device.hpp>

#include <webgpu/webgpu.h>

#include <string_view>
#include <iostream>
#include <utility>
#include <sstream>

using namespace j0o0ll;

void log_callback(LogLevel level, std::string_view message, void*) {
    std::string_view level_str;
    switch (level) {
        case LogLevel::Debug:
            level_str = "[DEBUG]";
            break;
        case LogLevel::Info:
            level_str = "[INFO]";
            break;
        case LogLevel::Warning:
            level_str = "[WARNING]";
            break;
        case LogLevel::Error:
            level_str = "[ERROR]";
            break;
    }

    std::ostream& output = level == LogLevel::Error ? std::cerr : std::cout;
    output << level_str << ": " << message << '\n';
}

int main() {
    // Log Callback
    set_log_callback(log_callback);

    // Instance
    auto instance_result = Instance::create();
    if (!instance_result.has_value()) {
        detail::emit_log(LogLevel::Error, instance_result.error().message);
        return 1;
    }

    Instance instance = std::move(instance_result).value();

    // Adapter
    WGPURequestAdapterOptions adapter_options = {};
    adapter_options.nextInChain = nullptr;

    auto adapter_result = Adapter::request(instance.get(), &adapter_options);
    if (!adapter_result.has_value()) {
        detail::emit_log(LogLevel::Error, adapter_result.error().message);
        return 1;
    }

    Adapter adapter = std::move(adapter_result).value();
    inspect_gpu(adapter.get());

    // Device
    WGPUDeviceDescriptor device_descriptor = WGPU_DEVICE_DESCRIPTOR_INIT;
    device_descriptor.nextInChain = nullptr;
    device_descriptor.label = to_wgpu_string_view("Pipus Abobora");
    device_descriptor.requiredFeatureCount = 0;
    device_descriptor.requiredFeatures = nullptr;
    device_descriptor.defaultQueue.label = to_wgpu_string_view("Pipus Abobora Queue");

    auto on_device_lost = [](
        WGPUDevice const*,
        WGPUDeviceLostReason reason,
        WGPUStringView message,
        void*, void*
    ) {
        std::ostringstream output;
        output 
            << "Device was lost: reason " << reason
            << " (" << to_std_string_view(message) << ")";

        detail::emit_log(LogLevel::Warning, output.str());
    };

    device_descriptor.deviceLostCallbackInfo.callback = on_device_lost;
    device_descriptor.deviceLostCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;

    auto on_device_error = [](
        WGPUDevice const*,
        WGPUErrorType type,
        WGPUStringView message,
        void*, void*
    ) {
        std::ostringstream output;
        output 
            << "Uncaptured device error: type " << type
            << " (" << to_std_string_view(message) << ")";

        detail::emit_log(LogLevel::Error, output.str());
    };

    device_descriptor.uncapturedErrorCallbackInfo.callback = on_device_error;

    auto device_result = Device::request(instance.get(), adapter.get(), &device_descriptor);
    if (!device_result.has_value()) {
        detail::emit_log(LogLevel::Error, device_result.error().message);
        return 1;
    }

    Device device = std::move(device_result).value();

    wgpuInstanceProcessEvents(instance.get());
    return 0;
}