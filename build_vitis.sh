#! /usr/bin/env bash

build_folder=detray-build-vitis
install_folder=detray-install-vitis

export CMAKE_EXPORT_COMPILE_COMMANDS="on"
export LOCAL_VECMEM_SOURCE=$(realpath vecmem)
export LOCAL_ALGEBRA_SOURCE=$(realpath algebra-plugins)

export CMAKE_EXPORT_COMPILE_COMMANDS="on"
cmake -S detray -B "$build_folder" --preset default-fp32 \
	-DCMAKE_INSTALL_PREFIX="$install_folder" \
	-DVECMEM_BUILD_VITIS_LIBRARY=ON \
    -DVECMEM_DEBUG_MSG_LVL=100 \
    -DDETRAY_VECMEM_SOURCE="SOURCE_DIR;$LOCAL_VECMEM_SOURCE" \
    -DDETRAY_ALGEBRA_PLUGINS_SOURCE="SOURCE_DIR;$LOCAL_ALGEBRA_SOURCE" \
	-G Ninja \
	-DDETRAY_BUILD_TUTORIALS=OFF \
	-DDETRAY_BUILD_TESTING=OFF \

cmake --build "$build_folder" -j 8 --target install
