/*
 * Copyright (c) 2016 Johan Sköld
 * License: https://opensource.org/licenses/ISC
 */

#include <sc.h>

#include <assert.h>     /* assert */
#include <stdint.h>     /* uintptr_t */

#include "fcontext.h"

/*
 * Compatibility
 */

#if defined(_MSC_VER)
#   define THREAD_LOCAL __declspec(thread)
#   define ALIGNOF(x)   __alignof(x)
#else
#   define THREAD_LOCAL __thread
#   define ALIGNOF(x)   __alignof__(x)
#endif

/*
 * Private implementation
 */

typedef struct sc_context {
    fcontext_t fctx;
    sc_context_proc_t proc;
} context_data;

THREAD_LOCAL context_data t_main;
THREAD_LOCAL context_data* t_current;

static uintptr_t align_down (uintptr_t addr, uintptr_t alignment) {
    assert(alignment > 0);
    assert((alignment & (alignment - 1)) == 0);
    return addr & ~(alignment - 1);
}

static void context_proc (transfer_t transfer) {
    context_data* data = (context_data*)transfer.data;
    assert(data != NULL);

    /* Jump back to parent */
    transfer = jump_fcontext(transfer.fctx, NULL);

    /* Update the current context */
    sc_current_context()->fctx = transfer.fctx;
    t_current = data;

    /* Execute the context proc */
    data->proc(transfer.data);
}

/*
 * Public implementation
 */

sc_context_t SC_CALL_DECL sc_context_create (
    void* stack_ptr,
    size_t stack_size,
    sc_context_proc_t proc
) {
    uintptr_t stack_addr;
    uintptr_t sp_addr;
    uintptr_t data_addr;
    fcontext_t fctx;
    context_data* data;

    assert(stack_ptr != NULL);
    assert(stack_size >= SC_MIN_STACK_SIZE);
    assert(proc != NULL);

    /* Determine the bottom of the stack */
    stack_addr = (uintptr_t)stack_ptr;
    sp_addr = stack_addr + stack_size;

    /* Reserve some space at the bottom for the context data */
    data_addr = sp_addr - sizeof(context_data);
    data_addr = align_down(data_addr, ALIGNOF(context_data));
    assert(data_addr > stack_addr);
    sp_addr = data_addr;

    /* Align the stack pointer to a 64-byte boundary */
    sp_addr = align_down(sp_addr, 64);
    assert(sp_addr > stack_addr);

    /* Determine the new stack size */
    stack_size = sp_addr - stack_addr;

    /* Create the context */
    fctx = make_fcontext((void*)sp_addr, stack_size, context_proc);
    assert(fctx != NULL);

    /* Create the context data at the reserved address */
    data = (context_data*)data_addr;
    data->proc = proc;

    /* Transfer the proc pointer to the context by briefly switching to it */
    data->fctx = jump_fcontext(fctx, data).fctx;
    return data;
}

void SC_CALL_DECL sc_context_destroy (sc_context_t context) {
    assert(context != sc_current_context());
    assert(context != sc_main_context());

    /* This doesn't actually do anything, but it's provided partly to get a
     * symmetric API, but mainly so we can easily do cleanup if need be in the
     * future. */

    (void)context;
}

void* SC_CALL_DECL sc_yield (sc_context_t target, void* value) {
    context_data* this_ctx = sc_current_context();
    transfer_t transfer;

    assert(target != NULL);

    if (target != this_ctx) {
        transfer = jump_fcontext(target->fctx, value);
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
