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

#define CL_TARGET_OPENCL_VERSION 210

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
#include <limits>
#include <memory>
#include <CL/cl.h>
#include <typeinfo>

typedef uint32_t size_type;
typedef uint8_t data_t;

auto generate_data() {
    // The new detector type
    using detector_t = detray::detector<detray::tutorial::my_metadata>;

    // First, create an empty detector in in host memory to be filled
    vecmem::host_memory_resource host_mr;
    detector_t det{host_mr};

    detray::toy_det_config toy_cfg{};
    // Number of barrel layers (0 - 4)
    toy_cfg.n_brl_layers(4u);
    // Number of endcap layers on either side (0 - 7)
    // Note: The detector must be configured with 4 barrel layers to be able to
    // add any encap layers
    toy_cfg.n_edc_layers(1u);

    // Now fill the detector
    return detray::build_toy_detector(host_mr, toy_cfg);
}

int main(int argc, char** argv) {
	// parse input arguments
    std::cout << argc << std::endl;
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <xclbin_file>" << std::endl; 
        return 1;
    }
    std::string binaryFile = argv[1];

    cl_int err;

    // Allocate input and output vectors
    size_type data_size = 12;
    size_type vector_size_bytes = sizeof(data_t) * data_size;

    std::vector<data_t> in(data_size);
    std::vector<data_t> out(data_size);

    vecmem::host_memory_resource host_mr;
    vecmem::vitis::device_memory_resource dev_mr;
    vecmem::vitis::host_memory_resource dev_host_mr;
    vecmem::vitis::copy vitis_cpy;

    auto [det_mng, names_mng] = detray::build_toy_detector(host_mr);

    auto det_fixed_buff = detray::get_buffer(det_mng, dev_mr, vitis_cpy);

	// ========[ OpenCL section ]========
	// Get the FPGA device
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

	// Get the binary file
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};

    OCL_CHECK(err, cl::Context context(device, nullptr, nullptr, nullptr, &err));
    OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

	// Program the binary onto the device
    std::cout << "Trying to program device " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    cl::Program program(context, {device}, bins, nullptr, &err);
    if (err != CL_SUCCESS) {
		std::cout << "Failed to program device with xclbin file!\n";
        exit(EXIT_FAILURE);
    } 
	std::cout << "Device: program successful!\n";
	OCL_CHECK(err, cl::Kernel kernel(program, "kernel_main", &err));

    // Allocate Buffer in Global Memory
    OCL_CHECK(err, cl::Buffer buffer_in(
				context, 
				CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 
				vector_size_bytes, 
				in.data(),
                &err)
	);
    OCL_CHECK(err, cl::Buffer buffer_out(
				context, 
				CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, 
				vector_size_bytes, 
				out.data(), 
				&err)
	);


	// Set the kernel arguments
    argc = 0;
    OCL_CHECK(err, err = kernel.setArg(argc++, buffer_in));
    OCL_CHECK(err, err = kernel.setArg(argc++, buffer_out));
    OCL_CHECK(err, err = kernel.setArg(argc++, data_size));

    cl::Event in_event;
    cl::Event kernel_event;
    cl::Event out_event;

	// Transfer data from host to device
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_in}, 0, NULL, &in_event));

	// Run the kernel
    OCL_CHECK(err, err = q.enqueueTask(kernel, NULL, &kernel_event));

	// Transfer result data from device to host
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_out}, CL_MIGRATE_MEM_OBJECT_HOST, NULL, &out_event))
    OCL_CHECK(err, err = q.finish());

    for (size_type i = 0; i < data_size; ++i) {
        std::cout << "in[" << i << "]: " << static_cast<int>(in[i]) << "\tout[" << i << "]: " << static_cast<int>(out[i]) << std::endl;
    }

    printf("ok\n");
}

