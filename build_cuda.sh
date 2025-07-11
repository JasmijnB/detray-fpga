#! /usr/bin/env bash

build_folder=detray-build-cuda
install_folder=detray-install-cuda

export CMAKE_EXPORT_COMPILE_COMMANDS="on"
cmake -S detray -B "$build_folder" --preset cuda \
	-DCMAKE_INSTALL_PREFIX="$install_folder" \
	-DDETRAY_BUILD_TUTORIALS=ON -G Ninja

cmake --build "$build_folder" -j 8 --target install
