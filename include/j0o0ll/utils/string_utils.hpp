#pragma once

#include <webgpu/webgpu.h>
#include <string_view>

namespace j0o0ll {
    inline std::string_view to_std_string_view(WGPUStringView wgpu_string_view) {
        return
            wgpu_string_view.data == nullptr
            ? std::string_view()
            : wgpu_string_view.length == WGPU_STRLEN
            ? std::string_view(wgpu_string_view.data)
            : std::string_view(wgpu_string_view.data, wgpu_string_view.length);
    }

    inline WGPUStringView to_wgpu_string_view(std::string_view std_string_view) {
        return { std_string_view.data(), std_string_view.size() };
    }

    inline WGPUStringView to_wgpu_string_view(const char* c_string) {
        return { c_string, WGPU_STRLEN };
    }
}