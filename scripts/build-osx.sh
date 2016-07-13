#!/bin/bash -ex

source "$(dirname "$BASH_SOURCE")/build-common.sh"

if [[ "$arch" == "universal" ]]; then
    arch_list="i386;x86_64"
    arch_cmds="-arch i386 -arch x86_64"
else
    arch_list="$arch"
    arch_cmds="-arch $arch"
fi

# Update cmake
brew update
if brew outdated --quiet | grep -qx cmake; then brew upgrade cmake; fi

# Build library and tests
mkdir build
cd build
cmake -DCMAKE_OSX_ARCHITECTURES="$arch_list" -DCMAKE_BUILD_TYPE="$config" ..
make VERBOSE=1

# Run tests
file bin/sc_tests
bin/sc_tests --reporter console

# Build and run examples
cd ../examples

for file in $(ls *.c); do
    example_name="${file%.*}"
    build_example "$example_name" $arch_cmds
    file "$example_name"
    "./$example_name" > /dev/null
done
