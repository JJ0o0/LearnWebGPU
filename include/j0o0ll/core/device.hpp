#pragma once

#include <j0o0ll/diagnostics/result.hpp>
#include <webgpu/webgpu.h>

namespace j0o0ll {
    class Device {
        public:
            static Result<Device> request(WGPUInstance instance, WGPUAdapter adapter, const WGPUDeviceDescriptor* descriptor);
            ~Device();

            Device(const Device&) = delete;
            Device& operator=(const Device&) = delete;

            Device(Device&& other) noexcept;
            Device& operator=(Device&& other) noexcept;

            WGPUDevice get() const noexcept;
        private:
            explicit Device(WGPUDevice device);
            WGPUDevice m_device = nullptr;
    };
}