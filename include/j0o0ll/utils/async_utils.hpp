#pragma once

#include <thread>

namespace j0o0ll {
    inline void sleep_for_milliseconds(unsigned int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
}