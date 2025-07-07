// Includes
#include <stdint.h>

extern "C" {
void kernel_main(uint32_t* in1, uint32_t* in2, uint32_t* out, int size) {
	for (uint32_t i = 0; i < size; i++) out[i] = in1[i] + in2[i];
}
}
