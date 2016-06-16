sc [![appveyor status](https://ci.appveyor.com/api/projects/status/github/rhoot/sc?branch=master&svg=true)](https://ci.appveyor.com/project/rhoot/sc/branch/master)
==

"Platform independent" co-routine library exposed through a minimal C API.
Future plans also include a type-safe C++ API.

I say "platform independent" since it *technically* supports a heap of
different platforms, but the projects are not yet set up to support them.

Currently supported compilers/configurations:

* VS2015 (x86, x86_64; `genie vs2015`)
* VS2013 (x86, x86_64; `genie vs2013`)
* MinGW (x86, x86_64; `genie --gcc=mingw gmake`)

Uses assembly from [boost::context] to do the heavy lifting.

Building
--------

Project generation is done through [GENie].

1. Ensure submodules are up to date by running `git submodule update --init`.
2. Download [GENie][GENie-dl].
3. Run `genie <action>` in the root folder to generate project files. Refer to
   the compiler list above for the supported actions, or `genie --help` for the
   full list.
4. Open up the **build** folder and build the projects.

To run the tests, build and run the **sc_tests** project.

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

[GENie]:            https://github.com/bkaradzic/GENie                  "bkaradzic/GENie"
[GENie-dl]:         https://github.com/bkaradzic/GENie#download-stable  "bkaradzic/GENie"
[boost::context]:   https://github.com/boostorg/context                 "boostorg/context"
