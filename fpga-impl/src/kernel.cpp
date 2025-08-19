#include <cstdint>
#include "detray/core/detector.hpp"
#include "detray/detectors/toy_metadata.hpp"
#include "vecmem/containers/device_vector.hpp"

namespace detray {
// Detector
using detector_host_t = detector<toy_metadata, host_container_types>;
using detector_device_t =
    detector<toy_metadata, device_container_types>;

using mask_id = typename detector_host_t::masks::id;
using acc_id = typename detector_host_t::accel::id;

constexpr std::uint64_t SIZE = 1024 * 1024;

extern "C" {
void kernel_main(const uint8_t * data, const uint8_t * detector_data, double * n_transforms) {
    for (int i = 0; i < SIZE; i++) {
	    vecmem::memory_buffer[i] = data[i];
    }

    constexpr std::size_t detector_size = sizeof(detector_device_t);
    uint8_t local_detector_data[sizeof(detector_device_t)];
    for (int i = 0; i < detector_size; i++) {
        local_detector_data[i] = detector_data[i];
    }
    const detector_device_t& det = *reinterpret_cast<detector_device_t*>(local_detector_data);

    n_transforms[0] = static_cast<double>(det.volumes().size());
    n_transforms[1] = static_cast<double>(det.transform_store().size());
    n_transforms[2] = static_cast<double>(det.mask_store().get<mask_id::e_rectangle2>().size());
    n_transforms[3] = static_cast<double>(det.mask_store().get<mask_id::e_trapezoid2>().size());
    n_transforms[4] = static_cast<double>(det.mask_store().get<mask_id::e_portal_ring2>().size());
    n_transforms[5] = static_cast<double>(det.mask_store().get<mask_id::e_portal_cylinder2>().size());
    n_transforms[6] = static_cast<double>(det.accelerator_store().get<acc_id::e_brute_force>().size());
    n_transforms[7] = static_cast<double>(det.accelerator_store().get<acc_id::e_disc_grid>().size());
    n_transforms[8] = static_cast<double>(det.accelerator_store().get<acc_id::e_cylinder2_grid>().size());

    n_transforms[9] = 123.5;
//    n_transforms[10] = det.transform_store()[0].point_to_global({0., 0., 0.})[1];
//    n_transforms[11] = det.transform_store()[0].point_to_global({0., 0., 0.})[2];
    for (unsigned int i = 0; i < 10; i++) {
        n_transforms[10 + 3*i] = det.transform_store()[i].point_to_global({0., 0., 0.})[0];
        n_transforms[10 + 3*i + 1] = det.transform_store()[i].point_to_global({0., 0., 0.})[1];
        n_transforms[10 + 3*i + 2] = det.transform_store()[i].point_to_global({0., 0., 0.})[2];
    }
}
}
}

