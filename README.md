sc [![Build status](https://github.com/rhoot/sc/workflows/Build/badge.svg?branch=master)](https://github.com/rhoot/sc/actions?query=workflow%3ABuild+branch%3Amaster) [![license](https://img.shields.io/github/license/rhoot/sc.svg)](https://opensource.org/licenses/ISC)
==

Cross platform co-routine library exposed through a minimal C API.

|         | arm | arm64           |mips | universal | x86 | x86_64 |
|---------|-----|-----------------|-----|-----------|-----|--------|
| Android |  ✓  |  ✓<sup>\*</sup> |  ✓  |           |  ✓  |    ✓   |
| FreeBSD |     |                 |     |           |     |    ✓   |
| iOS     |  ✓  |        ✓        |     |     ✓     |  ✓  |    ✓   |
| Linux   |  ✓  |        ✓        |     |           |  ✓  |    ✓   |
| OSX     |     |        ✓        |     |     ✓     |  ✓  |    ✓   |
| Windows |     |                 |     |           |  ✓  |    ✓   |

*<sup>\*</sup>: Builds properly, but not tested.*

See [the wiki] for build instructions.

License
-------

```
Copyright (c) 2016-2018, Johan Sköld

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
```

Third Party Software
--------------------

sc is based on code originating in [boost::context], which has the following
license:

```
Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
```

[boost::context]:   https://github.com/boostorg/context     "boostorg/context"
[the wiki]:         https://github.com/rhoot/sc/wiki        "sc wiki"
