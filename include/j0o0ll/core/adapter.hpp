#pragma once

#include <j0o0ll/diagnostics/result.hpp>
#include <webgpu/webgpu.h>

namespace j0o0ll {
    class Adapter {
        public:
            static Result<Adapter> request(WGPUInstance instance, const WGPURequestAdapterOptions* options);
            ~Adapter();

            Adapter(const Adapter&) = delete;
            Adapter& operator=(const Adapter&) = delete;

            Adapter(Adapter&& other) noexcept;
            Adapter& operator=(Adapter&& other) noexcept;

            WGPUAdapter get() const noexcept;
        private:
            explicit Adapter(WGPUAdapter adapter);
            WGPUAdapter m_adapter = nullptr;
    };
}