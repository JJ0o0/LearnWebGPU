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

void on_device_lost(WGPUDevice const*, WGPUDeviceLostReason reason, WGPUStringView message, void*, void*) {
    std::ostringstream output;
    output 
        << "Device was lost: reason " << reason
        << " (" << to_std_string_view(message) << ")";

    detail::emit_log(LogLevel::Warning, output.str());
}

void on_device_error(WGPUDevice const*, WGPUErrorType type, WGPUStringView message, void*, void*) {
    std::ostringstream output;
    output 
        << "Uncaptured device error: type " << type
        << " (" << to_std_string_view(message) << ")";

    detail::emit_log(LogLevel::Error, output.str());
}

WGPURequestAdapterOptions get_adapter_options() {
    WGPURequestAdapterOptions options = {};
    options.nextInChain = nullptr;

    return options;
}

WGPUDeviceDescriptor get_device_descriptor(const char* label, const char* default_queue_label) {
    WGPUDeviceDescriptor descriptor = WGPU_DEVICE_DESCRIPTOR_INIT;
    descriptor.nextInChain = nullptr;
    descriptor.label = to_wgpu_string_view(label);
    descriptor.requiredFeatureCount = 0;
    descriptor.requiredFeatures = nullptr;

    descriptor.defaultQueue.label = to_wgpu_string_view(default_queue_label);

    descriptor.deviceLostCallbackInfo.callback = on_device_lost;
    descriptor.deviceLostCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;

    descriptor.uncapturedErrorCallbackInfo.callback = on_device_error;

    return descriptor;
}

int main() {
    // Log Callback
    set_log_callback(log_callback);

    // Instance
    detail::emit_log(LogLevel::Info, "Creating Instance...");
        auto instance_result = Instance::create();
        if (!instance_result.has_value()) {
            detail::emit_log(LogLevel::Error, instance_result.error().message);
            return 1;
        }

        Instance instance = std::move(instance_result).value();
    detail::emit_log(LogLevel::Info, "Instance created successfully!");

    // Adapter
    detail::emit_log(LogLevel::Info, "Requesting Adapter...");
        WGPURequestAdapterOptions adapter_options = get_adapter_options();
        auto adapter_result = Adapter::request(instance.get(), &adapter_options);
        if (!adapter_result.has_value()) {
            detail::emit_log(LogLevel::Error, adapter_result.error().message);
            return 1;
        }

        Adapter adapter = std::move(adapter_result).value();
    detail::emit_log(LogLevel::Info, "Got Adapter!");
    inspect_gpu(adapter.get(), false);

    // Device
    detail::emit_log(LogLevel::Info, "Requesting Device...");
        WGPUDeviceDescriptor device_descriptor = get_device_descriptor("Pipous Abobora", "Pipous Abobora Queue");

        auto device_result = Device::request(instance.get(), adapter.get(), &device_descriptor);
        if (!device_result.has_value()) {
            detail::emit_log(LogLevel::Error, device_result.error().message);
            return 1;
        }

        Device device = std::move(device_result).value();
    detail::emit_log(LogLevel::Info, "Got Device!");

    wgpuInstanceProcessEvents(instance.get());
    return 0;
}