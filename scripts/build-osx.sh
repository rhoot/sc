#!/bin/bash -ex

source "$(dirname "$BASH_SOURCE")/build-common.sh"

if [[ "$arch" == "universal" ]]; then
    arch="i386;x86_64"
fi

# Update cmake
brew update
if brew outdated --quiet | grep -qx cmake; then brew upgrade cmake; fi

# Build
mkdir build
cd build
cmake -DCMAKE_OSX_ARCHITECTURES="$arch" -DCMAKE_BUILD_TYPE="$config" ..
make VERBOSE=1

# Test
file bin/sc_tests
bin/sc_tests --reporter console
