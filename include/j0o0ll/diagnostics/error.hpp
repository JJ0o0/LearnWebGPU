#pragma once

#include <cstdint>
#include <cstdlib>
#include <utility>
#include <string>

namespace j0o0ll {
    enum class ErrorCategory : std::uint8_t {
        Library,
        Instance,
        Adapter,
        Device,
        Resource,
        Capability,
        WebGPU
    };

    enum class ErrorCode : std::uint16_t {
        InstanceCreationFailed,

        AdapterRequestFailed,

        DeviceRequestFailed,
        DeviceLost,

        UnsupportedFeature,
        UnsupportedLimits,

        ResourceCreationFailed
    };

    constexpr ErrorCategory category(ErrorCode code) {
        switch (code) {
            case ErrorCode::InstanceCreationFailed:     return ErrorCategory::Instance;

            case ErrorCode::AdapterRequestFailed:       return ErrorCategory::Adapter;

            case ErrorCode::DeviceRequestFailed:        return ErrorCategory::Device;
            case ErrorCode::DeviceLost:                 return ErrorCategory::Device;

            case ErrorCode::UnsupportedFeature:         return ErrorCategory::Capability;
            case ErrorCode::UnsupportedLimits:          return ErrorCategory::Capability;

            case ErrorCode::ResourceCreationFailed:     return ErrorCategory::Resource;
        }

        std::abort();
    }

    struct Error {
        ErrorCode code;
        std::string message;

        Error(ErrorCode c, std::string m) : code(c), message(std::move(m)) {}

        ErrorCategory category() const {
            return j0o0ll::category(code);
        }
    };
}