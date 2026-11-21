#include <webgpu/webgpu.h>
#include <iostream>

int main() {
    // Primeiro, criamos um descriptor
    WGPUInstanceDescriptor instance_descriptor = {};
    instance_descriptor.nextInChain = nullptr;

    // Depois criamos uma instância usando esse descriptor
    WGPUInstance instance = wgpuCreateInstance(&instance_descriptor);
    if (!instance) {
        std::cerr << "Couldn't initialize WebGPU.\n";
        return 1;
    }

    // Mostrar o objeto
    std::cout << "WGPU Instance: " << instance << "\n";

    // Destruindo a instância
    wgpuInstanceRelease(instance);
    return 0;
}