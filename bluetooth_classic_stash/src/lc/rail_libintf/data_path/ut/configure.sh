#!/bin/bash
# Configuration script for building data_path_test

# Set paths
UTILS_PATH=../../../../utils

# Set compiler and flags
export CC=gcc
export CFLAGS="-Wall -Wextra -Werror -g -I../.. -I$UTILS_PATH"

# Optionally set build type
export BUILD_TYPE=Debug

# Print configuration
set -x

echo "Compiler: $CC"
echo "CFLAGS: $CFLAGS"
echo "Build type: $BUILD_TYPE"
echo "Utils path: $UTILS_PATH"

# Create and enter build directory
mkdir -p build
cd build

# Run CMake to configure the build, passing UTILS_PATH as a cache variable
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DUTILS_PATH=$UTILS_PATH ..
