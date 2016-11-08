#!/bin/bash -ex

# The toolchain file for linux-x64 is set up with the wrong asm compiler.
CMAKE_FLAGS="-DCMAKE_ASM_COMPILER=/usr/bin/x86_64-linux-gnu-gcc"

source "$(dirname "$0")/helpers.sh"
dockcross_build

# Run unit tests
build-debug/bin/sc_tests --reporter console
build-release/bin/sc_tests --reporter console

# Run examples
for file in $(ls examples/*.c); do
    ${file%.*} > /dev/null
done
