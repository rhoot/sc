/*
 * Copyright (c) 2016 Johan Sköld
 * License: https://opensource.org/licenses/ISC
 */

#pragma once

#include <stddef.h> /* size_t */

#if defined(_MSC_VER)
#   define SC_CALL_DECL __stdcall
#else
#   define SC_CALL_DECL
#endif

#if defined(__cplusplus)
extern "C" {
#endif

    /** Minimum supported stack size. */
    enum { SC_MIN_STACK_SIZE = 2048 };

    /** Context handle type. */
    typedef struct sc_context* sc_context_t;

    /** Context procedure type. */
    typedef void (SC_CALL_DECL * sc_context_proc_t) (void* param);

    /** Create a context with the given stack and procedure.
     **
     ** * `stack_ptr`:  Pointer to the buffer the context should use as stack.
     **                 Must be a valid pointer (not NULL).
     ** * `stack_size`: Size of the stack buffer provided in `stack_ptr`. Must
     **                 be at least `SC_MIN_STACK_SIZE`.
     ** * `proc`:       Procedure to invoke inside the new context. The
     **                 parameter passed to the proc will be the first value
     **                 yielded to the context through `sc_yield`.
     **
     ** **Note:** If the proc is allowed to run to its end, it will cause the
     **           process to exit.
     **
     ** **Important:** The stack must be big enough to be able to contain the
     **                maximum stack size used by the procedure. As this is
     **                implementation specific, it is up to the caller (or
     **                possibly attached debuggers) to ensure this is true. */
    sc_context_t SC_CALL_DECL sc_context_create (
        void* stack_ptr,
        size_t stack_size,
        sc_context_proc_t proc
    );

    /** Destroy a context created through `sc_context_create`.
     **
     ** * `context`: Context to destroy. Must not be the currently executing
     **              context, or the main context (retrieved by calling
     **                `sc_main_context`). */
    void SC_CALL_DECL sc_context_destroy (sc_context_t context);

    /** Switch execution to another context, returning control to it, and
     ** passing the given value to it. Returns the value passed to
     ** `sc_switch` or `sc_yield` when control is returned to this context.
     **
     ** * `target`: Context to switch control to. Must be a valid context
     **             created by `sc_context_create`, or returned by
     **             `sc_main_context`.
     ** * `value`: Value to pass to the target context. */
    void* SC_CALL_DECL sc_switch (sc_context_t target, void* value);

    /** Yields execution to the parent context, returning control to it, and
     ** passing the given value to it. Returns the value passed to `sc_switch`
     ** or `sc_yield` when control is returned to this context.
     **
     ** * `value`: Value to pass to the target context.
     **
     ** **Note:** The *parent context* is the context that created this
     **           context. It is up to the caller to ensure that the parent
     **           context is still valid.
     **
     ** **Important:** This should not be called from the main context
     **                (returned by `sc_main_context`), as it does not have a
     **                parent. Doing so triggers an assert in debug builds,
     **                and undefined behavior in release builds. */
    void* SC_CALL_DECL sc_yield (void* value);

    /** Get the handle for the currently executing context. */
    sc_context_t SC_CALL_DECL sc_current_context (void);

    /** Get the handle for the currently executing context's parent context.
     ** The parent context is the context that created this context. If the
     ** currently executing context is the main context, NULL is returned. */
    sc_context_t SC_CALL_DECL sc_parent_context (void);

    /** Get the handle for this thread's main context. */
    sc_context_t SC_CALL_DECL sc_main_context (void);

#if defined(__cplusplus)
} /* extern "C" */
#endif
