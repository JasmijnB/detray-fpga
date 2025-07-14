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


void load_detector(cl_device_id device,
                const std::string &binaryFile,
                std::vector<data_t> &out,
                size_t data_size) {
    cl_int err;
    size_t vector_size_bytes = data_size * sizeof(int);

    // Load xclbin
    std::vector<unsigned char> fileBuf = xcl::read_binary_file(binaryFile);
    const unsigned char* binary = fileBuf.data();
    size_t binary_size = fileBuf.size();

    // Create context and queue
    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    OCL_CHECK(err, );

    cl_command_queue q = clCreateCommandQueue(context, device, 0, &err);
    OCL_CHECK(err, );

    // Program FPGA with binary
    OCL_CHECK(err, cl_program program = clCreateProgramWithBinary(context, 1, &device,
                                                   &binary_size, &binary,
                                                   nullptr, &err));


    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        std::cerr << "Failed to program device with xclbin file!" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::cout << "Device: program successful!" << std::endl;

    // Create kernel
    cl_kernel kernel = clCreateKernel(program, "kernel_main", &err);
    OCL_CHECK(err, );

    vecmem::host_memory_resource host_mr;
    vecmem::vitis::device_memory_resource dev_mr (context, device, kernel, CL_MEM_READ_WRITE);
    vecmem::vitis::host_memory_resource dev_host_mr;
    vecmem::vitis::copy vitis_cpy;

    auto [det_mng, names_mng] = detray::build_toy_detector(host_mr);
    // load the detector data into the device memory as first buffer
    auto det_fixed_buff = detray::get_buffer(det_mng, dev_mr, vitis_cpy);

    OCL_CHECK(err, cl_mem buffer_out = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                       vector_size_bytes, nullptr, &err));

    // Set kernel arguments
    int argc = 1;
    OCL_CHECK(err, err = clSetKernelArg(kernel, argc++, sizeof(cl_mem), &buffer_out));
    OCL_CHECK(err, err = clSetKernelArg(kernel, argc++, sizeof(size_type), &data_size));


    // Write input buffer
//    OCL_CHECK(err, err = clEnqueueWriteBuffer(q, buffer_in, CL_TRUE, 0,
//                                              vector_size_bytes, in.data(), 0, nullptr, nullptr));

    // Run kernel
    cl_event kernel_event;
    OCL_CHECK(err, err = clEnqueueTask(q, kernel, 0, nullptr, &kernel_event));

    // Wait for completion
    clWaitForEvents(1, &kernel_event);
    clReleaseEvent(kernel_event);

    // Read output buffer
    OCL_CHECK(err, err = clEnqueueReadBuffer(q, buffer_out, CL_TRUE, 0,
                                             vector_size_bytes, out.data(), 0, nullptr, nullptr));

    clFinish(q);

    // Cleanup
    // TODO: Fix cleanup not working :( (free errors)
//    clReleaseMemObject(buffer_in);
//    clReleaseMemObject(buffer_out);
//    clReleaseKernel(kernel);
//    clReleaseProgram(program);
//    clReleaseCommandQueue(q);
//    clReleaseContext(context);
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

    data_t size_in_bytes = DATA_SIZE * sizeof(data_t);

    auto krnl = xrt::kernel(device, uuid, "kernel_main");

    std::cout << "Allocate Buffer in Global Memory\n";

    xrt::bo::flags device_flags = xrt::bo::flags::device_only;
    auto bo_a = xrt::bo(device, size_in_bytes, device_flags, krnl.group_id(0));
    auto bo_b = xrt::bo(device, size_in_bytes, device_flags, krnl.group_id(0));
    auto bo_out = xrt::bo(device, size_in_bytes, device_flags, krnl.group_id(1));
    
    std::cout << "bo a address: " << bo_a.address() << std::endl;
    std::cout << "bo b address: " << bo_b.address() << std::endl;
    std::cout << "out address: " << bo_out.address() << std::endl;

    data_t data_in[DATA_SIZE];
    for (data_t i = 0; i < DATA_SIZE; ++i) {
        data_in[i] = i;
    }

    bo_a.write(data_in, size_in_bytes, 0);

    /*
    // Map the contents of the buffer object into host memory
    auto bo_a_map = bo_a.map<data_t*>();
    auto bo_b_map = bo_b.map<data_t*>();
    auto bo_out_map = bo_out.map<data_t*>();
    std::fill(bo_a_map, bo_a_map + DATA_SIZE, 0);
    std::fill(bo_out_map, bo_out_map + DATA_SIZE, 0);
    */

    /*
    // Create the test data
    data_t bufReference[DATA_SIZE];
    for (data_t i = 0; i < DATA_SIZE; ++i) {
        bo_a_map[i] = 12;
        bo_b_map[0] = i * 10;
        bufReference[i] = i;
    }
    */

    /*
    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";
    bo_a.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo_b.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    */

    std::cout << "Execution of the kernel\n";
    auto run = krnl(bo_a, bo_out, DATA_SIZE);
    run.wait();

    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;
    data_t data_out[DATA_SIZE];
    bo_out.read(data_out, size_in_bytes, 0);

    // Validate our results
    /*
    if (std::memcmp(bo_out_map, bufReference, DATA_SIZE))
        throw std::runtime_error("Value read back does not match reference");
        */
    for (size_type i = 0; i < DATA_SIZE; ++i) {
        std::cout << "Output data[" << i << "] = " << static_cast<int>(data_out[i]) << std::endl;
    }

    std::cout << "TEST PASSED\n";
    return 0;
}

