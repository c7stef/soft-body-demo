#!/bin/bash

cmake \
    -DCMAKE_TOOLCHAIN_FILE:FILEPATH=$(pwd)/x86_64-linux-gcc.cmake \
    -DCMAKE_BUILD_TYPE:STRING="$1" \
    -S . -B build/

cmake --build build/ --target all -j 8 --
