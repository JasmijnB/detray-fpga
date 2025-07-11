#! /usr/bin/env bash

install_folder="./detray-install-default"
build_folder="./detray-build-default"

export CMAKE_EXPORT_COMPILE_COMMANDS="on"
export LOCAL_VECMEM_SOURCE=$(realpath vecmem)

cmake -S detray -B "$build_folder" --preset default-fp32 \
	-DCMAKE_INSTALL_PREFIX="$install_folder" \
	-DVECMEM_BUILD_VITIS_LIBRARY=ON \
    -DDETRAY_VECMEM_SOURCE="SOURCE_DIR;$LOCAL_VECMEM_SOURCE" \
	-G Ninja \
	-DDETRAY_BUILD_TUTORIALS=ON
# 	-DDETRAY_BUILD_TESTING=OFF \

cmake --build "$build_folder" -j 8 --target install
