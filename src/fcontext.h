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

    /* For the provided fcontext implementations, there's no necessary work to
       be done for freeing a context, but some custom backends (for proprietary
       hardware) do. */
#if defined(SC_CUSTOM_FREE_FCONTEXT)
    void free_fcontext (fcontext_t);
#else
    static void free_fcontext (fcontext_t fctx) { (void)fctx; }
#endif

#if defined(__cplusplus)
} /* extern "C" */
#endif
