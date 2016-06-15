//---
// Copyright (c) 2016 Johan Sköld
// License: https://opensource.org/licenses/ISC
//---

#pragma once

extern "C" {

    using fcontext_t = void*;

    struct transfer_t {
        fcontext_t fctx;
        void* data;
    };

    transfer_t jump_fcontext (fcontext_t to, void* vp);
    fcontext_t make_fcontext (void* sp, size_t size, void(*fn)(transfer_t));
    transfer_t ontop_fcontext (fcontext_t to, void* vp, transfer_t(*fn)(transfer_t));

} // extern "C"
