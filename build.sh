#! /usr/bin/env bash

export CMAKE_EXPORT_COMPILE_COMMANDS="on"
cmake -S detray -B detray-build --preset local-vecmem \
	-DDETRAY_BUILD_TUTORIALS=ON -G Ninja

# cmake --build detray-build -j 8
