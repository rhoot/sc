//---
// Copyright (c) 2016 Johan Sköld
// License: https://opensource.org/licenses/ISC
//---

#pragma once

#include <stddef.h>

#if defined(_MSC_VER)
#   define SJ_CALL_DECL __stdcall
#else
#   define SJ_CALL_DECL
#endif

#if defined(__cplusplus)
extern "C" {
#endif

    /// Minimum supported stack size.
    enum { SJ_MIN_STACK_SIZE = 2048 };

    /// Context handle type.
    typedef struct sj_context* sj_context_t;

    /// Context procedure type.
    typedef void (SJ_CALL_DECL * sj_context_proc_t) (void* param);

    /// Create a context with the given stack and procedure.
    ///
    /// * `stack_ptr`:  Pointer to the buffer the context should use as stack.
    /// * `stack_size`: Size of the context stack.
    /// * `proc`:       Procedure to invoke inside the new context. The
    ///                 parameter passed to the proc will be the first value
    ///                 yielded to the context through `sj_yield`.
    ///
    /// **Important:** The stack must be big enough to be able to contain the
    ///                maximum stack size used by the procedure.
    sj_context_t SJ_CALL_DECL sj_context_create (
        void* stack_ptr,
        size_t stack_size,
        sj_context_proc_t proc
    );

    /// Destroy a context created through `sj_context_create`.
    ///
    /// * `context`: Context to destroy.
    ///
    /// **Important:** The passed context must *not* be the currently executing
    ///                context, or the main context (retrieved by calling
    ///                `sj_main_context`).
    void SJ_CALL_DECL sj_context_destroy (sj_context_t context);

    /// Yield execution to another context, returning control to it, and
    /// passing the given value to it. Returns the value passed to
    /// `sj_yield` when control is returned to the calling context.
    ///
    /// * `target`: Context to switch control to.
    /// * `value`: Value to pass to the target context.
    void* SJ_CALL_DECL sj_yield (sj_context_t target, void* value);

    /// Get the handle for the currently executing context.
    sj_context_t SJ_CALL_DECL sj_current_context ();

    /// Get the handle for this thread's main context.
    sj_context_t SJ_CALL_DECL sj_main_context ();

#if defined(__cplusplus)
} // extern "C"
#endif
