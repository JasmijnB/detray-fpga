cmake_minimum_required(VERSION 3.16)
project(FPGAHost LANGUAGES CXX)
include( CMakeFindDependencyMacro )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})

set(CL_TARGET_OPENCL_VERSION 110)

set(CMAKE_PREFIX_PATH ../detray-install-default)
find_package(vecmem REQUIRED)
find_package(algebra-plugins REQUIRED)
find_package(actsvg REQUIRED)
find_package(covfie REQUIRED)
find_package(detray REQUIRED)

set(XILINX_XRT $ENV{XILINX_XRT})

include_directories(${XILINX_XRT}/include)
link_directories(${XILINX_XRT}/lib)
set(XRT_LIBS xrt_coreutil)

# Including the cmake modules from the Vitis Install.
#set(CMAKE_MODULE_PATH
# "$ENV{XILINX_VITIS}/vitis-server/scripts/cmake"
# ${CMAKE_MODULE_PATH}
#)

include_directories(include)

# find_package(Vitis REQUIRED)

add_executable(host src/host.cpp)

target_link_libraries(host
	detray::core_array
	vecmem::core
	vecmem::vitis
	OpenCL
	${XRT_LIBS}
)

get_target_property(MAIN_CFLAGS host COMPILE_OPTIONS)
message("-- Target compiler flags are: ${MAIN_CFLAGS}")

# ==================[ KERNEL ]==================

set(HOST_FILE ${SRC_DIR}/host.cpp)
set(KERNEL_FILE ${SRC_DIR}/fpga_kernel.cpp)
set(CONFIG_FILE ${SRC_DIR}/config.cfg)
set(LINK_CONFIG_FILE ${SRC_DIR}/link.cfg)

set(HOST_OUTPUT "host")
set(SYNTH_OUTPUT "kernel.xo")
set(KERNEL_XCLBIN "kernel.xclbin")

set(PLATFORM "xilinx_u50_gen3x16_xdma_5_202210_1")

add_custom_target(kernel_hw
    COMMAND v++ -I/home/jbookelm/fpga_detray/detray-install/include
            -t hw
            -k kernel_main
            --platform ${PLATFORM}
            -DDETRAY_ALGEBRA_ARRAY
            -DDETRAY_CUSTOM_SCALARTYPE=float
            ../src/kernel.cpp -o ${SYNTH_OUTPUT}

    COMMENT "Building the kernel for hw mode."
)

add_custom_target(kernel_hw_emu
    COMMAND v++ -I/home/jbookelm/fpga_detray/detray-install/include
            -t hw_emu
            -k kernel_main
            --platform ${PLATFORM}
            -DDETRAY_ALGEBRA_ARRAY
            -DDETRAY_CUSTOM_SCALARTYPE=float
            ../src/kernel.cpp -o kernel_emu.xo

    COMMENT "Building the kernel for hw emulation mode."
)

get_target_property(vecmem_vitis_location vecmem::vitis LOCATION)
message(STATUS "vecmem::vitis points to: ${vecmem_vitis_location}")
