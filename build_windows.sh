#!/bin/bash
set -e

BUILD_DIR="build-windows"
TOOLCHAIN_FILE="toolchain-windows.cmake"

cat > $TOOLCHAIN_FILE << 'EOF'
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
EOF

mkdir -p $BUILD_DIR && cd $BUILD_DIR
cmake  -DENABLE_DEBUG=ON  -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ../cmake/windows

make -j$(nproc)

echo "build for windows $BUILD_DIR/"