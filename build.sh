#!/bin/bash
set -e

BUILD_DIR="build-linux"

mkdir -p $BUILD_DIR && cd $BUILD_DIR
cmake -G Ninja ../cmake/linux/ -DENABLE_DEBUG=ON
ninja -j $(nproc)
echo "build for Linux $BUILD_DIR/"