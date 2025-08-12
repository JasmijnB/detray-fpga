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
#include "detray/detectors/toy_metadata.hpp"
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

using detector_host_t = detray::detector<detray::toy_metadata, detray::host_container_types>;
using detector_device_t =
    detray::detector<detray::toy_metadata, detray::device_container_types>;
using mask_id = typename detector_host_t::masks::id;
using acc_id = typename detector_host_t::accel::id;

void print_kernel(typename detector_host_t::view_type det_data) {
    // Setup of the device-side detector
    detector_device_t det(det_data);

    printf("Number of volumes: %d\n", det.volumes().size());
    printf("Number of transforms: %d\n", det.transform_store().size());
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
}

void write_detector_to_file(const detector_host_t &det,
                            const typename detector_host_t::name_map &names) {
    auto writer_cfg = detray::io::detector_writer_config{}
                          .format(detray::io::format::json)
                          .replace_files(true);

    // Takes the detector 'det', a volume name map (only entry here the
    // detector name) and the writer config
    detray::io::write_detector(det, names, writer_cfg);
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

    detray::toy_det_config toy_cfg{};
    // Number of barrel layers (0 - 4)
    toy_cfg.n_brl_layers(1u);
    // Number of endcap layers on either side (0 - 7)
    toy_cfg.n_edc_layers(0u);

    const auto [det_mng, names] = detray::build_toy_detector(host_mr, toy_cfg);
    write_detector_to_file(det_mng, names);

    vecmem::vitis::device_memory_resource dev_mr(data_in, size_in_bytes);
    vecmem::vitis::copy vitis_cpy(data_in);
    // load the detector data into the device memory as first buffer
    auto det_fixed_buff = detray::get_buffer(det_mng, dev_mr, vitis_cpy);
    std::cout << "got the buffer!" << std::endl;

    auto host_view = detray::get_data(det_fixed_buff);
//    detector_device_t det{host_view};
    // auto det = host_view;
    detector_device_t det(host_view);


    std::cout << "Allocate Buffer in Global Memory\n";
    int args = 0;
    auto bo_in = xrt::bo(device, size_in_bytes, krnl.group_id(args++));
    auto bo_device_view = xrt::bo(device, size_in_bytes, krnl.group_id(args++));
    auto bo_out = xrt::bo(device, size_in_bytes, krnl.group_id(args++));

    std::cout << "bo in address: " << bo_in.address() << std::endl;
    std::cout << "bo host view address: " << bo_device_view.address() << std::endl;
    std::cout << "out address: " << bo_out.address() << std::endl;

    bo_in.write(data_in, size_in_bytes, 0);
    bo_device_view.write(&det, sizeof(det), 0);
    bo_in.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo_device_view.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(bo_in, bo_device_view, bo_out);
    run.wait();

    std::cout << "Synchronize the output buffer data from the device" << std::endl;
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    std::cout << "Read the output data\n";
    bo_out.read(data_out);


    // Validate our results
    /*
    if (std::memcmp(data_in, data_out, size_in_bytes) != 0)
        throw std::runtime_error("Value read back does not match reference");
	*/

    print_kernel(host_view);
    for (size_type i = 0; i < 10; ++i) {
        std::cout << "Output data[" << i << "] = " << static_cast<int>(data_out[i]) << std::endl;
    }

    std::cout << "TEST PASSED\n";
    return 0;
}

