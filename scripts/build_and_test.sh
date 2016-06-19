#!/bin/bash -ex

function get_genie {
    local OS="$1"

    if [[ "$OS" == "osx" ]]; then
        local OS="darwin"
    fi

    wget "https://github.com/bkaradzic/bx/raw/master/tools/bin/$OS/genie"
    chmod +x genie
}

function do_build {
    local OS="$1"
    local CONFIG="$2"

    if [[ "$OS" == "osx" ]]; then
        ./genie --clang=osx gmake
        make -C build/gmake-clang-osx config="$CONFIG"
    elif [[ "$OS" == "linux" ]]; then
        ./genie --gcc=linux gmake
        make -C build/gmake-gcc-linux config="$CONFIG"
    else
        echo "Unsupported OS" > /dev/stderr
        exit 1
    fi
}

function run_tests {
    bin/sc_tests --reporter console
}

get_genie "$1"
do_build "$1" "$2"
run_tests
