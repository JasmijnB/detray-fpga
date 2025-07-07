#! /usr/bin/env bash

export CMAKE_EXPORT_COMPILE_COMMANDS="on"
cmake -S detray -B detray-build --preset local-vecmem \
	-DCMAKE_INSTALL_PREFIX="./detray-install" \
	-DDETRAY_BUILD_TUTORIALS=ON -G Ninja

cmake --build detray-build -j 8 --target install
