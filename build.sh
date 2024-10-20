#!/bin/bash

cmake \
    -DCMAKE_TOOLCHAIN_FILE=$(pwd)/x86_64-linux-gcc.cmake \
    -S . -B build/

cmake --build build/ -j 4
