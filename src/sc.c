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

/* Apple doesn't support __thread on arm platforms, and they have a bug in the
 * x86 support, so we need to fall back to pthreads for these platforms. */
#if defined(__APPLE__) && (defined(__arm__) || defined(__arm64__) || defined(__i386__))
#   include <pthread.h>
#   include <stdlib.h>  /* malloc, free */
#   define USE_PTHREAD_SPECIFICS
#endif

/*
 * Private implementation
 */

typedef struct sc_context {
    fcontext_t fctx;
    sc_context_proc_t proc;
    sc_context_t parent;
} context_data;

#if defined(USE_PTHREAD_SPECIFICS)
    static pthread_key_t t_main;
    static pthread_key_t t_current;

    static void cleanup_main (void* main) {
        free(main);
    }

    static void do_init_main_key (void) {
        int error = pthread_key_create(&t_main, cleanup_main);
        assert(error == 0);
        (void)error;
    }

    static void init_main_key (void) {
        static pthread_once_t s_once = PTHREAD_ONCE_INIT;
        pthread_once(&s_once, do_init_main_key);
    }

    static void do_init_current_key (void) {
        int error = pthread_key_create(&t_current, NULL);
        assert(error == 0);
        (void)error;
    }

    static void init_current_key (void) {
        static pthread_once_t s_once = PTHREAD_ONCE_INIT;
        pthread_once(&s_once, do_init_current_key);
    }

    static context_data* get_main (void) {
        init_main_key();

        void* data = pthread_getspecific(t_main);

        if (!data) {
            data = malloc(sizeof(context_data));
            assert(data != NULL);
            pthread_setspecific(t_main, data);
        }

        return (context_data*)data;
    }

    static void set_current (context_data* context) {
        init_current_key();
        pthread_setspecific(t_current, context);
    }

    static context_data* get_current (void) {
        init_current_key();
        return (context_data*)pthread_getspecific(t_current);
    }
#else
    static THREAD_LOCAL context_data t_main;
    static THREAD_LOCAL context_data* t_current;

    static context_data* get_main (void) {
        return &t_main;
    }

    static void set_current (context_data* context) {
        t_current = context;
    }

    static context_data* get_current (void) {
        return t_current;
    }
#endif

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
    set_current(data);

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
    data->parent = sc_current_context();

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

void* SC_CALL_DECL sc_switch (sc_context_t target, void* value) {
    context_data* this_ctx = sc_current_context();
    transfer_t transfer;

    assert(target != NULL);

    if (target != this_ctx) {
        transfer = jump_fcontext(target->fctx, value);
        sc_current_context()->fctx = transfer.fctx;
        set_current(this_ctx);
        value = transfer.data;
    }

    return value;
}

void* SC_CALL_DECL sc_yield (void* value) {
    context_data* current = sc_current_context();
    assert(current->parent != NULL);
    return sc_switch(current->parent, value);
}

sc_context_t SC_CALL_DECL sc_current_context (void) {
    context_data* current = get_current();
    return current ? current : get_main();
}

sc_context_t SC_CALL_DECL sc_parent_context (void) {
    return sc_current_context()->parent;
}

sc_context_t SC_CALL_DECL sc_main_context (void) {
    return get_main();
}
