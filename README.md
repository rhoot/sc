sc [![travis-ci status](https://travis-ci.org/rhoot/sc.svg?branch=master)](https://travis-ci.org/rhoot/sc) [![appveyor status](https://ci.appveyor.com/api/projects/status/github/rhoot/sc?branch=master&svg=true)](https://ci.appveyor.com/project/rhoot/sc/branch/master)
==

Cross platform co-routine library exposed through a minimal C API.

|         | arm | arm64 |mips | universal | x86 | x86_64 |
|---------|-----|-------|-----|-----------|-----|--------|
| Android |  ✓  |       |  ✓  |           |  ✓  |    ✓   |
| FreeBSD |     |       |     |           |     |    ✓   |
| iOS     |  ✓  |   ✓   |     |     ✓     |  ✓  |    ✓   |
| Linux   |  ✓  |       |     |           |  ✓  |    ✓   |
| OSX     |     |       |     |     ✓     |  ✓  |    ✓   |
| Windows |     |       |     |           |  ✓  |    ✓   |

See [the wiki] for build instructions.

Based on modified assembly from [boost::context].

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

[boost::context]:   https://github.com/boostorg/context     "boostorg/context"
[the wiki]:         https://github.com/rhoot/sc/wiki        "sc wiki"
