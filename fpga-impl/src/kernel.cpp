// Includes
#include <stdint.h>
#include <iostream>
// #include "detray/core/detector.hpp"

extern "C" {
void kernel_main(uint8_t* in, uint8_t* out, int32_t size) {
    for (int i = 0; i < size; ++i) {
        std::cout << "in[" << i << "]: " << static_cast<int>(in[i]) << std::endl;
        out[i] = in[i];
    }
}
}
