#!/bin/bash -ex

build_example() {
    ./dockcross bash -c "\$CC -Wall -Werror -I/work/include -L/work/build-release/lib $1.c -lsc -o $1"
}

# Check to see if we've got some special thing to do for this target
if [[ -f "scripts/ci/$target.sh" ]]; then
    source "scripts/ci/$target.sh"
fi

# Extract the compiler helper script from the docker container
docker run --rm "dockcross/$target" > dockcross
chmod +x dockcross

./dockcross cc --version
./dockcross c++ --version
./dockcross cmake --version

# Generate build files, and build
mkdir build-debug
mkdir build-release
./dockcross cmake $CMAKE_FLAGS -DCMAKE_BUILD_TYPE=Debug -B/work/build-debug -H/work
./dockcross cmake $CMAKE_FLAGS -DCMAKE_BUILD_TYPE=Release -B/work/build-release -H/work
./dockcross make -C /work/build-debug VERBOSE=1
./dockcross make -C /work/build-release VERBOSE=1

# Build examples
for file in $(ls examples/*.c); do
    example_name=${file%.*}
    build_example "$example_name"
    file "$example_name"
done

# For linux-x64 also run unit tests and examples
if [[ "$target" == "linux-x86" ]] || [[ "$target" == "linux-x64" ]]; then

    # We run them through docker, cause for some reason travis can't run the
    # x86 tests.
    ./dockcross /work/build-debug/bin/sc_tests --reporter console
    ./dockcross /work/build-release/bin/sc_tests --reporter console

    for file in $(ls examples/*.c); do
        ./dockcross /work/${file%.*} > /dev/null
    done

fi
