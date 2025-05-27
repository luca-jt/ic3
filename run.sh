#! /usr/bin/bash

cmake -S . -B build -D CMAKE_BUILD_TYPE=Release
cmake --build build
./build/ic3 ./benchmarks/unsafe/test2.aig
