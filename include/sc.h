//---
// Copyright (c) 2016 Johan Sköld
// License: https://opensource.org/licenses/ISC
//---

#pragma once

#include <stddef.h>     // size_t

#if defined(_MSC_VER)
#   define SC_CALL_DECL __stdcall
#else
#   define SC_CALL_DECL
#endif

#if defined(__cplusplus)
extern "C" {
#endif

    /// Minimum supported stack size.
    enum { SC_MIN_STACK_SIZE = 2048 };

    /// Context handle type.
    typedef struct sc_context* sc_context_t;

    /// Context procedure type.
    typedef void (SC_CALL_DECL * sc_context_proc_t) (void* param);

    /// Create a context with the given stack and procedure.
    ///
    /// * `stack_ptr`:  Pointer to the buffer the context should use as stack.
    /// * `stack_size`: Size of the context stack.
    /// * `proc`:       Procedure to invoke inside the new context. The
    ///                 parameter passed to the proc will be the first value
    ///                 yielded to the context through `sc_yield`.
    ///
    /// **Important:** The stack must be big enough to be able to contain the
    ///                maximum stack size used by the procedure.
    sc_context_t SC_CALL_DECL sc_context_create (
        void* stack_ptr,
        size_t stack_size,
        sc_context_proc_t proc
    );

    /// Destroy a context created through `sc_context_create`.
    ///
    /// * `context`: Context to destroy.
    ///
    /// **Important:** The passed context must *not* be the currently executing
    ///                context, or the main context (retrieved by calling
    ///                `sc_main_context`).
    void SC_CALL_DECL sc_context_destroy (sc_context_t context);

    /// Yield execution to another context, returning control to it, and
    /// passing the given value to it. Returns the value passed to
    /// `sc_yield` when control is returned to the calling context.
    ///
    /// * `target`: Context to switch control to.
    /// * `value`: Value to pass to the target context.
    void* SC_CALL_DECL sc_yield (sc_context_t target, void* value);

    /// Get the handle for the currently executing context.
    sc_context_t SC_CALL_DECL sc_current_context ();

    /// Get the handle for this thread's main context.
    sc_context_t SC_CALL_DECL sc_main_context ();

#if defined(__cplusplus)
} // extern "C"
#endif
