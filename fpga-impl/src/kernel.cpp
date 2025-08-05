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

void print_kernel(typename detector_host_t::view_type det_data) {
    // Setup of the device-side detector
    detector_device_t det(det_data);

    printf("Number of volumes: %d\n", det.volumes().size());
    printf("Number of transforms: %d\n", det.transform_store().size());

    /*
    printf("Number of rectangles: %d\n",
           det.mask_store().get<mask_id::e_rectangle2>().size());
    printf("Number of trapezoids: %d\n",
           det.mask_store().get<mask_id::e_trapezoid2>().size());
    printf("Number of portal discs: %d\n",
           det.mask_store().get<mask_id::e_portal_ring2>().size());
    printf("Number of portal cylinders: %d\n",
           det.mask_store().get<mask_id::e_portal_cylinder2>().size());
    printf("Number of portal collections: %d\n",
           det.accelerator_store().get<acc_id::e_brute_force>().size());
    printf("Number of disc grids: %d\n",
           det.accelerator_store().get<acc_id::e_disc_grid>().size());
    printf("Number of cylinder grids: %d\n",
           det.accelerator_store().get<acc_id::e_cylinder2_grid>().size());
           */
}

#define HOST_SIZE 1000

// using detector_device_t =
//     detector<detray::toy_metadata, device_container_types>;
extern "C" {
void kernel_main(uint8_t * data, uint8_t * host_view_data, uint8_t * n_transforms) {
    constexpr std::size_t h_view_type_size = sizeof(detector_host_t::view_type);
    uint8_t local_host_view_data[h_view_type_size];
    for (int i = 0; i < h_view_type_size; i++) {
        local_host_view_data[i] = host_view_data[i];
    }
    detector_host_t::view_type host_view = * (detector_device_t *) local_host_view_data;
//    detector_device_t det(host_view);
    n_transforms[0] = det.transform_store().size();
}
}
}



