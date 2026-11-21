cmake_minimum_required(VERSION 3.20)
project(
    LearnWebGPU
    VERSION 0.1.0
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(WEBGPU_BACKEND "DAWN" CACHE STRING "WebGPU backend")
add_subdirectory(external/webgpu)

add_executable(${CMAKE_PROJECT_NAME}
    "src/main.cpp"

    "src/core/instance.cpp"
    "src/core/adapter.cpp"
    "src/core/device.cpp"

    "src/diagnostics/inspect.cpp"
    "src/diagnostics/log.cpp"
)

target_include_directories(${CMAKE_PROJECT_NAME}
    PRIVATE
        "include/"
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        webgpu
)

if(MSVC)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

target_copy_webgpu_binaries(${CMAKE_PROJECT_NAME})