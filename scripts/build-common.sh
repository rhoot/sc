#!/bin/bash

# travis_retry is a function, so to be able to use it from within a script
# file, we need to copy the implementation.
travis_retry() {
    local result=0
    local count=1
    while [ $count -le 3 ]; do
        [ $result -ne 0 ] && {
            echo -e "\n${ANSI_RED}The command \"$@\" failed. Retrying, $count of 3.${ANSI_RESET}\n" >&2
        }
        "$@"
        result=$?
        [ $result -eq 0 ] && break
        count=$(($count + 1))
        sleep 1
    done

    [ $count -gt 3 ] && {
        echo -e "\n${ANSI_RED}The command \"$@\" failed 3 times.${ANSI_RESET}\n" >&2
    }

    return $result
}

build_example() {
    local name="$1"
    shift 1
    cc -Wall -Werror $@ -I../include -L../build/lib "$name.c" -lsc -o "$name"
}
