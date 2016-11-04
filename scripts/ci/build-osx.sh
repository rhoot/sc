#!/bin/bash -ex

if [[ "$arch" == "universal" ]]; then
    arch_list="i386;x86_64"
    arch_cmds="-arch i386 -arch x86_64"
else
    arch_list="$arch"
    arch_cmds="-arch $arch"
fi

build_example() {
    cc -Wall -Werror $arch_cmds -Iinclude -Lbuild/lib "$1.c" -lsc -o "$1"
}

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
cd ..

for file in $(ls examples/*.c); do
    example_name="${file%.*}"
    build_example "$example_name"
    file "$example_name"
    "$example_name" > /dev/null
done
