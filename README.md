sc [![travis-ci status](https://travis-ci.org/rhoot/sc.svg?branch=master)](https://travis-ci.org/rhoot/sc) [![appveyor status](https://ci.appveyor.com/api/projects/status/github/rhoot/sc?branch=master&svg=true)](https://ci.appveyor.com/project/rhoot/sc/branch/master)
==

Cross platform co-routine library exposed through a minimal C API.

Supported platforms and compilers:

* Android (arm)
* Linux (x86, x86_64)
* MinGW (x86, x86_64)
* OSX (x86, x86_64, universal)
* Visual Studio 2010+ (x86, x86_64)

Based on assembly from [boost::context].

Building
--------

Build script generation is done through [CMake]. To generate scripts
appropriate for your current system, simply run the below.

    mkdir build
    cd build
    cmake ..

All build related files will go into the newly created build direcetory, which
may safely be deleted at any time. If you want to specify your preferred build
script type, you may do so by using CMake's `-G` parameter to change the
generator. Refer to `cmake --help` for a list of the available generators.

### Defines

Some defines may be used to modify the build.

#### CMAKE_OSX_ARCHITECTURES

    cmake -DCMAKE_OSX_ARCHITECTURES=<...> ..

**OSX only.** Replace `<...>` above with a semicolon-separated list of
architectures to include in the resulting binaries (i386, x86_64).

#### SC_FORCE_32BIT

    cmake -DSC_FORCE_32BIT=1 ..

**GCC compatible compilers only.** Force building of a 32-bit binary even when
on a 64-bit system. For OSX, prefer using the `CMAKE_OSX_ARCHITECTURES` define
instead.

### Android

To build for android, [Andrey Kamaev's toolchain file][android-cmake] is
included in this repository to simplify the process. You will need to know
where the Android NDK is located on your hard drive, as well as what API level
to compile for.

    mkdir build-android
    cd build-android
    cmake -DCMAKE_TOOLCHAIN_FILE=../3rdparty/android-cmake/android.toolchain.cmake \
          -DCMAKE_BUILD_TYPE=<configuration> \
          -DANDROID_NDK=<ndk path> \
          -DANDROID_NATIVE_API_LEVEL=<api level> \
          ..
    make

Replace `<configuration>` above with one of `Debug` and `Release`, `<ndk path>`
with the path to your installed NDK, and `<api level>` with your desired API
level.

License
-------

Copyright (c) 2016, Johan Sköld

Permission to use, copy, modify, and/or distribute this software for any  
purpose with or without fee is hereby granted, provided that the above  
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES  
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF  
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR  
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES  
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN  
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF  
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

[android-cmake]:    https://github.com/taka-no-me/android-cmake         "taka-no-me/android-cmake"
[boost::context]:   https://github.com/boostorg/context                 "boostorg/context"
[CMake]:            https://cmake.org                                   "CMake"
