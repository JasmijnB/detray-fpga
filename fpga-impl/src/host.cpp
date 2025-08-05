/** Detray library, part of the ACTS project (R&D line)
 *
 * (c) 2023-2024 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

// TODO: Remove this when gcc fixes their false positives.
#include <istream>
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic warning "-Warray-bounds"
#endif

#define CL_TARGET_OPENCL_VERSION 120
// Project include(s)
#include "detray/builders/cuboid_portal_generator.hpp"
#include "detray/builders/surface_factory.hpp"
#include "detray/builders/volume_builder.hpp"
#include "detray/core/detector.hpp"
#include "detray/definitions/geometry.hpp"
#include "detray/definitions/units.hpp"
#include "detray/detectors/build_toy_detector.hpp"
#include "detray/io/frontend/detector_writer.hpp"
#include "detray/navigation/volume_graph.hpp"
#include "detray/geometry/shapes/rectangle2D.hpp"


// Example include(s)
#include "detray/tutorial/detector_metadata.hpp"
#include "detray/tutorial/square_surface_generator.hpp"
#include "detray/tutorial/types.hpp"  // linear algebra types

// Vecmem include(s)
#include <vecmem/memory/host_memory_resource.hpp>
#include <vecmem/memory/vitis/device_memory_resource.hpp>
#include <vecmem/memory/vitis/host_memory_resource.hpp>

#include <vecmem/utils/vitis/copy.hpp>
#include <vecmem/utils/xcl2.hpp>

// System include(s)
#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"

typedef uint32_t size_type;
typedef uint8_t data_t;
#define DATA_SIZE 12

using detector_host_t = detray::detector<detray::tutorial::my_metadata, detray::host_container_types>;
using detector_device_t =
    detray::detector<detray::tutorial::my_metadata, detray::device_container_types>;
using mask_id = typename detector_host_t::masks::id;
using acc_id = typename detector_host_t::accel::id;

auto build_detector(vecmem::host_memory_resource &host_mr) {
    // The new detector type

    // First, create an empty detector in in host memory to be filled
    detector_host_t det{host_mr};

    // Now fill the detector

    // Get a generic volume builder first and decorate it later
    detray::volume_builder<detector_host_t> vbuilder{detray::volume_id::e_cuboid};

    // Fill some squares into the volume
    using square_factory_t =
        detray::surface_factory<detector_host_t, detray::tutorial::square2D>;
    auto sq_factory = std::make_shared<square_factory_t>();

    // Add some programmatically generated square surfaces
    auto sq_generator =
        std::make_shared<detray::tutorial::square_surface_generator>(
            10, 10.f * detray::unit<detray::scalar>::mm);

    // Add a portal box around the cuboid volume with a min distance of 'env'
    constexpr auto env{0.1f * detray::unit<detray::scalar>::mm};
    auto portal_generator =
        std::make_shared<detray::cuboid_portal_generator<detector_host_t>>(env);

    // Add surfaces to volume and add the volume to the detector
    vbuilder.add_surfaces(sq_factory);
    vbuilder.add_surfaces(sq_generator);
    vbuilder.add_surfaces(portal_generator);

    vbuilder.build(det);
    return det;
}

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


int main(int argc, char** argv) {
	// parse input arguments
    std::cout << argc << std::endl;
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <xclbin_file>" << std::endl; 
        return 1;
    }
    std::string binaryFile = argv[1];

    // Allocate input and output vectors
    size_type data_size = 12;
    size_type vector_size_bytes = sizeof(data_t) * data_size;

    std::vector<data_t> out(data_size);
    int device_index = 0;
    std::cout << "Open the device" << device_index << std::endl;
    auto device = xrt::device(device_index);
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);


    auto krnl = xrt::kernel(device, uuid, "kernel_main");

    std::cout << "Allocate Buffer in Global Memory\n";
    // randomly choose a big number
    constexpr std::uint32_t size_in_bytes = 1024 * 1024;
    std::uint8_t data_in[size_in_bytes];
    std::uint8_t data_out[size_in_bytes];


    vecmem::host_memory_resource host_mr;

    auto det_mng = build_detector(host_mr);
//    const auto [det_mng, names] = detray::build_toy_detector(host_mr);
    vecmem::vitis::device_memory_resource dev_mr(data_in, size_in_bytes);
    vecmem::vitis::copy vitis_cpy(data_in);
    // load the detector data into the device memory as first buffer
    auto det_fixed_buff = detray::get_buffer(det_mng, dev_mr, vitis_cpy);
    std::cout << "got the buffer!" << std::endl;

    auto host_view = detray::get_data(det_fixed_buff);
    print_kernel(host_view);

    std::cout << "Allocate Buffer in Global Memory\n";
    int args = 0;
    auto bo_in = xrt::bo(device, size_in_bytes, krnl.group_id(args++));
    auto bo_host_view = xrt::bo(device, size_in_bytes, krnl.group_id(args++));
    auto bo_out = xrt::bo(device, size_in_bytes, krnl.group_id(args++));

    std::cout << "bo in address: " << bo_in.address() << std::endl;
    std::cout << "bo host view address: " << bo_host_view.address() << std::endl;
    std::cout << "out address: " << bo_out.address() << std::endl;

    bo_in.write(data_in, size_in_bytes, 0);
    bo_host_view.write(&host_view, sizeof(host_view), 0);
    bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo_host_view.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(bo_in, bo_out, size_in_bytes);
    run.wait();

    std::cout << "Synchronize the output buffer data from the device" << std::endl;
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    std::cout << "Read the output data\n";
    bo_out.read(data_out);


    // Validate our results
    if (std::memcmp(data_in, data_out, size_in_bytes) != 0)
        throw std::runtime_error("Value read back does not match reference");

    /*
    for (size_type i = 0; i < size_in_bytes; ++i) {
        std::cout << "Output data[" << i << "] = " << static_cast<int>(data_out[i]) << std::endl;
    }
    */

    std::cout << "TEST PASSED\n";
    return 0;
}

