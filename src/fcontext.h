/*
 * Copyright (c) 2016 Johan Sköld
 * License: https://opensource.org/licenses/ISC
 */

#pragma once

#include <stddef.h> /* size_t */

#if defined(__cplusplus)
extern "C" {
#endif

    typedef void* fcontext_t;

    typedef struct {
        fcontext_t fctx;
        void* data;
    } transfer_t;

    transfer_t jump_fcontext (fcontext_t to, void* vp);
    fcontext_t make_fcontext (void* sp, size_t size, void(*fn)(transfer_t));

#if defined(__cplusplus)
} /* extern "C" */
#endif
