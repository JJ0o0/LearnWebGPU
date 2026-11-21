#pragma once

#include <j0o0ll/diagnostics/log.hpp>
#include <string_view>

namespace j0o0ll::detail {
    void emit_log(LogLevel level, std::string_view message);
}