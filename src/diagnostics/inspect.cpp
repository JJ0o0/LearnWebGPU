#include <j0o0ll/diagnostics/inspect.hpp>

#include <j0o0ll/diagnostics/detail/emit.hpp>
#include <j0o0ll/diagnostics/log.hpp>

#include <j0o0ll/utils/string_utils.hpp>

#include <sstream>

namespace j0o0ll {
    void inspect_gpu(WGPUAdapter adapter, bool list_features) {
        std::ostringstream output;

        // Adapter Properties
        WGPUAdapterInfo properties = WGPU_ADAPTER_INFO_INIT;
        wgpuAdapterGetInfo(adapter, &properties);

        output << "GPU Information:\n"
               << " |=> Vendor ID: " << properties.vendorID << '\n'
               << " |=> Vendor Name: " << to_std_string_view(properties.vendor) << '\n'
               << " |=> Architecture: " << to_std_string_view(properties.architecture) << '\n'
               << " |=> Device ID: " << properties.deviceID << '\n'
               << " |=> Name: " << to_std_string_view(properties.device) << '\n'
               << " |=> Driver Description: " << to_std_string_view(properties.description) << '\n'
               << " |=> Adapter Type: " << properties.adapterType << '\n'
               << " |=> Backend Type: " << properties.backendType;

        detail::emit_log(LogLevel::Info, output.str());
        output.str("");
        output.clear();

        wgpuAdapterInfoFreeMembers(properties);

        // Adapter Limits
        WGPULimits limits = WGPU_LIMITS_INIT;
        if (wgpuAdapterGetLimits(adapter, &limits) == WGPUStatus_Success) {
            output << "GPU Limits:\n"
                   << " |=> MaxTextureDimension1D: " << limits.maxTextureDimension1D << '\n'
                   << " |=> MaxTextureDimension2D: " << limits.maxTextureDimension2D << '\n'
                   << " |=> MaxTextureDimension3D: " << limits.maxTextureDimension3D << '\n'
                   << " |=> MaxTextureArrayLayers: " << limits.maxTextureArrayLayers;

            detail::emit_log(LogLevel::Info, output.str());
            output.str("");
            output.clear();
        } else {
            detail::emit_log(LogLevel::Warning, "Failed to retrieve GPU limits.");
        }

        // Adapter Features
        WGPUSupportedFeatures features = WGPU_SUPPORTED_FEATURES_INIT;
        wgpuAdapterGetFeatures(adapter, &features);

        output << "Found " << features.featureCount << " GPU features";

        if (list_features) {
            output << ":";
            for (size_t i = 0; i < features.featureCount; ++i) {
                output << "\n |=> 0x" << std::hex << features.features[i] << std::dec;
            }
        } else {
            output << ".";
        }

        detail::emit_log(LogLevel::Info, output.str());

        wgpuSupportedFeaturesFreeMembers(features);
    }
}