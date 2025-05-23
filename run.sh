#! /usr/bin/bash

cmake --build build
./build/ic3 ./benchmarks/unsafe/test1.aig
