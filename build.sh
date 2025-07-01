#! /usr/bin/env bash

cmake -S detray -B detray-build --preset dev-fp32 \
-DDETRAY_BUILD_TUTORIALS=ON -G Ninja

cmake --build detray-build -j 8
