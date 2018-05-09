/*
 * Copyright (c) 2016 Johan Sköld
 * License: https://opensource.org/licenses/ISC
 */

#pragma once

#include <stddef.h> /* size_t */

#include <sc.h>

typedef void* sc_context_sp_t;

typedef struct {
    sc_context_sp_t ctx;
    void* data;
} sc_transfer_t;

SC_EXTERN sc_transfer_t SC_CALL_DECL sc_jump_context (sc_context_sp_t to, void* vp);
SC_EXTERN sc_context_sp_t SC_CALL_DECL sc_make_context (void* sp, size_t size, void(*fn)(sc_transfer_t));

/* sc_context_state is only implemented for Windows and macOS atm. */

#if defined(_WIN32)
#   define SC_HAS_CONTEXT_STATE_IMPL
#elif defined(__APPLE__) && defined(__MACH__)
#   define SC_HAS_CONTEXT_STATE_IMPL
#endif

#if defined(SC_HAS_CONTEXT_STATE_IMPL)
    SC_EXTERN void SC_CALL_DECL sc_context_state (sc_state_t* state, sc_context_sp_t ctx);
#else
    static void sc_context_state (sc_state_t* state, sc_context_sp_t ctx) {
        (void)ctx;
        state->type = SC_CPU_TYPE_UNKNOWN;
    }
#endif

/* For the provided fcontext implementations, there's no necessary work to
   be done for freeing a context, but some custom backends (for proprietary
   hardware) do. */

#if defined(SC_CUSTOM_FREE_CONTEXT)
    SC_EXTERN void SC_CALL_DECL sc_free_context (sc_context_sp_t);
#else
    static void sc_free_context (sc_context_sp_t ctx) { (void)ctx; }
#endif
