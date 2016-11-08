osx_build_example() {
    local name="$1"
    shift 1
    cc -Wall -Werror $EXAMPLE_BUILD_ARGS -Iinclude -Lbuild-release/lib "$name.c" -lsc -o "$name"
}

osx_build() {
    # Update cmake
    brew update
    if brew outdated --quiet | grep -qx cmake; then brew upgrade cmake; fi

    # Generate build files, and build
    mkdir build-debug
    mkdir build-release
    cmake $CMAKE_FLAGS -DCMAKE_BUILD_TYPE=Debug -Bbuild-debug -H.
    cmake $CMAKE_FLAGS -DCMAKE_BUILD_TYPE=Release -Bbuild-release -H.
    make -C build-debug VERBOSE=1
    make -C build-release VERBOSE=1

    # Build examples
    for file in $(ls examples/*.c); do
        example_name=${file%.*}
        osx_build_example "$example_name"
        file "$example_name"
    done
}

