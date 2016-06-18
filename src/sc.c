//---
// Copyright (c) 2016 Johan Sköld
// License: https://opensource.org/licenses/ISC
//---

#include <sc.h>

#include <assert.h>     // assert
#include <stdbool.h>    // bool, true, false
#include <stdlib.h>     // exit

#include "fcontext.h"

//
// Compatibility macros
//

#if defined(_MSC_VER)
#   define THREAD_LOCAL __declspec(thread)
#   define ALIGNOF(x)   __alignof(x)
#   define ASSUME(x)    __assume(x)
#else
#   define THREAD_LOCAL __thread
#   define ALIGNOF(x)   __alignof__(x)
#   define ASSUME(x)                        \
        do {                                \
            if (!(x)) {                     \
                __builtin_unreachable();    \
            }                               \
        } while (false)
#endif

//
// Assertion override, purely for unit testing purposes.
//

void (*g_sc_assert) ();

#if defined(NDEBUG)
#   define ASSERT(expr)  ASSUME(expr)
#else // !defined(NDEBUG)
#   define ASSERT(expr)                     \
        do {                                \
            if (g_sc_assert) {              \
                if (!(expr)) {              \
                    g_sc_assert();          \
                }                           \
            } else {                        \
                assert(expr);               \
            }                               \
        } while (false)
#endif // !defined(NDEBUG)

//
// Private implementation
//

typedef struct sc_context {
    fcontext_t fctx;
    sc_context_proc_t proc;
} context_data;

THREAD_LOCAL context_data t_main;
THREAD_LOCAL context_data* t_current;

static uintptr_t align_down (uintptr_t addr, uintptr_t alignment) {
    ASSERT(alignment > 0);
    ASSERT((alignment & (alignment - 1)) == 0);
    return addr & ~(alignment - 1);
}

static void context_proc (transfer_t transfer) {
    context_data* data = (context_data*)transfer.data;
    ASSERT(data != NULL);

    // Jump back to parent
    transfer = jump_fcontext(transfer.fctx, NULL);

    // Update the current context
    sc_current_context()->fctx = transfer.fctx;
    t_current = data;

    // Execute the context proc
    data->proc(transfer.data);

    // TODO: If a proc gets to run to its end we should exit the thread, not the process.
    exit(0);
}

//
// Public implementation
//

sc_context_t SC_CALL_DECL sc_context_create (
    void* stack_ptr,
    size_t stack_size,
    sc_context_proc_t proc
) {
    ASSERT(stack_ptr != NULL);
    ASSERT(stack_size >= SC_MIN_STACK_SIZE);
    ASSERT(proc != NULL);

    // Determine the bottom of the stack
    uintptr_t stack_addr = (uintptr_t)stack_ptr;
    uintptr_t sp_addr = stack_addr + stack_size;

    // Reserve some space at the bottom for the context data
    uintptr_t data_addr = sp_addr - sizeof(context_data);
    data_addr = align_down(data_addr, ALIGNOF(context_data));
    ASSERT(data_addr > stack_addr);
    sp_addr = data_addr;

    // Align the stack pointer to a 64-byte boundary
    sp_addr = align_down(sp_addr, 64);
    ASSERT(sp_addr > stack_addr);

    // Determine the new stack size
    stack_size = sp_addr - stack_addr;

    // Create the context
    fcontext_t fctx = make_fcontext((void*)sp_addr, stack_size, context_proc);
    ASSERT(fctx != NULL);

    // Create the context data at the reserved address
    context_data* data = (context_data*)data_addr;
    data->proc = proc;

    // Transfer the proc pointer to the context by briefly switching to it
    data->fctx = jump_fcontext(fctx, data).fctx;
    return data;
}

void SC_CALL_DECL sc_context_destroy (sc_context_t context) {
    ASSERT(context != sc_current_context());
    ASSERT(context != sc_main_context());

    // This doesn't actually do anything, but it's provided partly to get a
    // symmetric API, but mainly so we can easily do cleanup if need be in the
    // future.
}

void* SC_CALL_DECL sc_yield (sc_context_t target, void* value) {
    ASSERT(target != NULL);

    sc_context_t this_ctx = sc_current_context();

    if (target != this_ctx) {
        transfer_t transfer = jump_fcontext(target->fctx, value);
        sc_current_context()->fctx = transfer.fctx;
        t_current = this_ctx;
        value = transfer.data;
    }

    return value;
}

sc_context_t SC_CALL_DECL sc_current_context () {
    return t_current ? t_current : &t_main;
}

sc_context_t SC_CALL_DECL sc_main_context () {
    return &t_main;
}
