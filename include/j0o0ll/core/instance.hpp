#pragma once

#include <j0o0ll/diagnostics/result.hpp>
#include <webgpu/webgpu.h>

namespace j0o0ll {
    class Instance {
        public:
            static Result<Instance> create();
            ~Instance();

            Instance(const Instance&) = delete;
            Instance& operator=(const Instance&) = delete;

            Instance(Instance&& other) noexcept;
            Instance& operator=(Instance&& other) noexcept;

            WGPUInstance get() const noexcept;
        private:
            explicit Instance(WGPUInstance instance);
            WGPUInstance m_instance = nullptr;
    };
}