#include <cstdint>
#include "detray/core/detector.hpp"
#include "detray/tutorial/detector_metadata.hpp"

namespace detray {
// Detector
using detector_host_t = detector<tutorial::my_metadata, host_container_types>;
using detector_device_t =
    detector<tutorial::my_metadata, device_container_types>;

using mask_id = typename detector_host_t::masks::id;
using acc_id = typename detector_host_t::accel::id;

extern "C" {
void kernel_main(uint8_t * data, uint8_t * detector_data, uint8_t * n_transforms) {
    constexpr std::size_t detector_size = sizeof(detector_device_t);
    uint8_t local_detector_data[sizeof(detector_device_t)];
    for (int i = 0; i < detector_size; i++) {
        local_detector_data[i] = detector_data[i];
    }
    const detector_device_t& det = *reinterpret_cast<detector_device_t*>(local_detector_data);
    n_transforms[0] = det.transform_store().size();
    n_transforms[1] = det.volumes().size();
    n_transforms[2] = det.mask_store().get<mask_id::e_square2>().size();
    n_transforms[3] = det.mask_store().get<mask_id::e_trapezoid2>().size();
    n_transforms[4] = det.mask_store().get<mask_id::e_portal_rectangle2>().size();
    n_transforms[5] = det.accelerator_store().get<acc_id::e_brute_force>().size();
}
}
}



