#!/bin/bash -ex

source "$(dirname "$BASH_SOURCE")/build-common.sh"

mkdir build

if [[ "$arch" == "arm" ]]; then

    # Pull the cross-compiler container
    export RPXC_IMAGE=rhoot/rpxc-cmake
    docker pull $RPXC_IMAGE

    # Extract the helper script
    docker run $RPXC_IMAGE > rpxc
    chmod +x rpxc

    # Generate makefiles and build
    ./rpxc --args '-w /build/build' cmake -DCMAKE_TOOLCHAIN_FILE=../scripts/cmake/linux-arm.cmake -DCMAKE_BUILD_TYPE="$config" ..
    ./rpxc --args '-w /build/build' make VERBOSE=1

else

    # Update cmake
    CMAKE_URL="http://www.cmake.org/files/v3.3/cmake-3.3.2-Linux-x86_64.tar.gz"
    mkdir cmake && travis_retry wget --no-check-certificate --quiet -O - ${CMAKE_URL} | tar --strip-components=1 -xz -C cmake
    export PATH="$(pwd)/cmake/bin:$PATH"

    # Generate makefiles
    cd build

    if [[ "$arch" == "i386" ]]; then
        cmake -DCMAKE_BUILD_TYPE="$config" -DSC_FORCE_32BIT=1 ..
    else
        cmake -DCMAKE_BUILD_TYPE="$config" ..
    fi

    # Build and test
    make VERBOSE=1
    file bin/sc_tests
    bin/sc_tests --reporter console

    # Build and run examples
    cd ../examples

    for file in $(ls *.c); do
        example_name="${file%.*}"

        if [[ "$arch" == "i386" ]]; then
            build_example "$example_name" -m32
        else
            build_example "$example_name"
        fi

        file "$example_name"
        "./$example_name" > /dev/null
    done

fi
