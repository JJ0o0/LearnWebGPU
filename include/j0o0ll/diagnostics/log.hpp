#pragma once

#include <string_view>
#include <cstdint>

namespace j0o0ll {
    enum class LogLevel : std::uint8_t {
        Debug,
        Info,
        Warning,
        Error
    };

    using LogCallback = void(*)(LogLevel level, std::string_view message, void* user);
    void set_log_callback(LogCallback callback, void* user = nullptr);
}