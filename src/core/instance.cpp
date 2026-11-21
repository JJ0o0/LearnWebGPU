#include <j0o0ll/core/instance.hpp>
#include <j0o0ll/diagnostics/result.hpp>
#include <j0o0ll/diagnostics/error.hpp>

#include <webgpu/webgpu.h>
#include <utility>

namespace j0o0ll {
    Instance::Instance(WGPUInstance instance) 
        : m_instance(instance) {}

    Instance::~Instance() {
        if (m_instance) {
            wgpuInstanceRelease(m_instance);
        }
    }

    Instance::Instance(Instance&& other) noexcept 
        : m_instance(std::exchange(other.m_instance, nullptr)) {}
    
    Instance& Instance::operator=(Instance&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        if (m_instance) {
            wgpuInstanceRelease(m_instance);
        }

        m_instance = std::exchange(other.m_instance, nullptr);
        return *this;
    }

    WGPUInstance Instance::get() const noexcept {
        return m_instance;
    }

    Result<Instance> Instance::create() {
        WGPUInstanceDescriptor descriptor = {};
        descriptor.nextInChain = nullptr;

        WGPUInstance instance = wgpuCreateInstance(&descriptor);
        if (!instance) {
            return Error(
                ErrorCode::InstanceCreationFailed,
                "Failed to create WebGPU Instance."
            );
        }

        return Instance(instance);
    }
}