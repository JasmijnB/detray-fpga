cmake_minimum_required( VERSION 3.26.5 )
project( CheckSYCL LANGUAGES CXX )
list( INSERT CMAKE_MODULE_PATH 0       "/home/jbookelm/fpga_detray/detray-install/lib64/cmake/vecmem-1.13.0"       "/home/jbookelm/fpga_detray/detray-install/lib64/cmake/vecmem-1.13.0/hip"       "/home/jbookelm/fpga_detray/detray-install/lib64/cmake/vecmem-1.13.0/sycl" )
enable_language( SYCL )
file( WRITE "${CMAKE_CURRENT_BINARY_DIR}/result.cmake"
   "set( CMAKE_SYCL_COMPILER \"${CMAKE_SYCL_COMPILER}\" )" )