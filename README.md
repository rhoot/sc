sc [![travis-ci status](https://travis-ci.org/rhoot/sc.svg?branch=master)](https://travis-ci.org/rhoot/sc) [![appveyor status](https://ci.appveyor.com/api/projects/status/github/rhoot/sc?branch=master&svg=true)](https://ci.appveyor.com/project/rhoot/sc/branch/master)
==

Cross platform co-routine library exposed through a minimal C API.

Based on assembly from [boost::context].

Building
--------

Project generation is done through [GENie][GENie-dl].

#### Linux (x86, x86_64)

1. Generate makefiles.

        genie --gcc=linux gmake

2. Build your preferred configuration.

        make -C build/gmake-gcc-linux config=debug32
        make -C build/gmake-gcc-linux config=debug64
        make -C build/gmake-gcc-linux config=release32
        make -C build/gmake-gcc-linux config=release64

#### MinGW (x86, x86_64)

1. Generate makefiles. MinGW-w64's binaries are assumed to be in the `%PATH%`.

        genie --gcc=mingw gmake

2. Build your preferred configuration.

        make -C build/gmake-gcc-mingw config=debug32
        make -C build/gmake-gcc-mingw config=debug64
        make -C build/gmake-gcc-mingw config=release32
        make -C build/gmake-gcc-mingw config=release64

#### OSX (x86, x86_64)

1. Generate makefiles.

        genie --clang=osx gmake

2. Build your preferred configuration.

        make -C build/gmake-clang-osx config=debug32
        make -C build/gmake-clang-osx config=debug64
        make -C build/gmake-clang-osx config=release32
        make -C build/gmake-clang-osx config=release64

#### Visual Studio (x86, x86_64)

1. Generate project files for your preferred version of Visual Studio.

        genie vs2010
        genie vs2012
        genie vs2013
        genie vs2015
        genie vs15

2. Open and build the `build\vs<version>\sc.sln` file.

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

[GENie-dl]:         https://github.com/bkaradzic/GENie#download-stable  "bkaradzic/GENie"
[boost::context]:   https://github.com/boostorg/context                 "boostorg/context"
