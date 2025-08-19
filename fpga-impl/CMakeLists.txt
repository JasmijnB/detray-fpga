cmake_minimum_required(VERSION 3.16)
project(FPGAHost LANGUAGES CXX) 
include( CMakeFindDependencyMacro )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SRC_DIR "../src")
set(CONFIG_DIR "../config")
set(WORK_DIR ".")

set(HOST_FILE ${SRC_DIR}/host.cpp)
set(KERNEL_FILE ${SRC_DIR}/kernel.cpp)

set(CONFIG_FILE ${CONFIG_DIR}/config.cfg)
set(LINK_CONFIG_FILE ${CONFIG_DIR}/link.cfg)

set(HOST_OUTPUT "host")
set(SYNTH_OUTPUT "kernel_main.xo")
set(KERNEL_XCLBIN "kernel.xclbin")
set(PLATFORM "xilinx_u50_gen3x16_xdma_5_202210_1")

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

include_directories(include)

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
set(HLS_CONFIG ${CONFIG_DIR}/hls.cfg)
set(LINK_CONFIG ${CONFIG_DIR}/link.cfg)
set(HW_EMU_XCLBIN ${WORK_DIR}/hw_emu/${KERNEL_XCLBIN})
set(CSIM_BIN ${WORK_DIR}/kernel_main)

set(COMMON_OPTIONS --config ${HLS_CONFIG} --work_dir ${WORK_DIR})
set(V_INCLUDE_FLAGS := -I /home/jbookelm/fpga_detray/detray-install-default/include)

add_custom_command(
    OUTPUT ${SYNTH_OUTPUT}
    COMMAND v++ 
        --compile  
        --mode hls  
        --platform=${PLATFORM} 
        ${COMMON_OPTIONS} 

    DEPENDS ${KERNEL_FILE}
    COMMENT "Synthesizing the kernel source"
)

add_custom_command(
    OUTPUT ${HW_EMU_XCLBIN}
    COMMAND v++ 
        -l 
        -t hw_emu 
        --config ${LINK_CONFIG} ${SYNTH_OUTPUT} 
        -o ${HW_EMU_XCLBIN}
    DEPENDS ${SYNTH_OUTPUT}
    COMMENT "Linking hardware emulation XCLBIN: ${HW_EMU_XCLBIN}"
)

add_custom_target(link_hw_emu DEPENDS ${HW_EMU_XCLBIN})

add_custom_target(run_hw_emu
	COMMAND ${CMAKE_COMMAND} -E env XCL_EMULATION_MODE=hw_emu ./host ./hw_emu/${KERNEL_XCLBIN}
    WORKING_DIRECTORY ${WORK_DIR}
    COMMENT "Running hardware emulation"
)
