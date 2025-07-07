cmake_minimum_required(VERSION 3.16)
project(FPGAHost LANGUAGES CXX)
include( CMakeFindDependencyMacro )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCL REQUIRED)
include_directories(${OpenCL_INCLUDE_DIRS})

set(CMAKE_PREFIX_PATH ../detray-install)
find_package(vecmem REQUIRED)
find_package(algebra-plugins REQUIRED)
find_package(actsvg REQUIRED)
find_package(detray REQUIRED)

set(XILINX_XRT $ENV{XILINX_XRT})
include_directories(${XILINX_XRT}/include)
link_directories(${XILINX_XRT}/lib)
set(XRT_LIBS xrt_coreutil)

add_executable(host src/host.cpp)

target_link_libraries(host
	detray::core_array
	vecmem::core
	OpenCL::OpenCL
	${XRT_LIBS}
)

get_target_property(MAIN_CFLAGS host COMPILE_OPTIONS)
message("-- Target compiler flags are: ${MAIN_CFLAGS}")
