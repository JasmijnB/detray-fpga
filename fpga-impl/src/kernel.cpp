// Includes
#include <stdint.h>
#include <iostream>
// #include "detray/core/detector.hpp"

#define DATA_SIZE 12

extern "C" {
void kernel_main(int64_t in_ptr, int64_t out_ptr) {
    int8_t * in = (int8_t *) in_ptr;
    int8_t * out = (int8_t *) out_ptr;
#pragma HLS interface mode=m_axi port=in
#pragma HLS interface mode=m_axi port=out
    
    for (int i = 0; i < DATA_SIZE; ++i) {
        out[i] = in[i];
    }
}
}
