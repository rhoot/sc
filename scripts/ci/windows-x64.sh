#!/bin/bash -ex

# The toolchain file for Windows-x64 doesn't have the correct system details
CMAKE_FLAGS="-DCMAKE_SYSTEM_NAME=Windows -DCMAKE_SYSTEM_PROCESSOR=AMD64"

source "$(dirname "$0")/dockcross.sh"
dockcross_build
