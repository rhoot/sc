//---
// Copyright (c) 2016 Johan Sköld
// License: https://opensource.org/licenses/ISC
//---

#include <sc.h>

#include <cassert>
#include <cstdint>
#include <memory>

#include "fcontext.hpp"

//
// VS2013 compatibility
//

#if defined(_MSC_VER) && _MSC_VER <= 1800
#   define thread_local __declspec(thread)
#   define alignof(x)   __alignof(x)
#endif

//
// Assumptions
//

#if defined(_MSC_VER)
#   define SC_ASSUME(x) __assume(x)
#else // !defined(_MSC_VER)
#   define SC_ASSUME(x)                                 \
        do {                                            \
            if (!(x)) {                                 \
                __builtin_unreachable();                \
            }                                           \
        } while (0,0)
#endif // !defined(_MSC_VER)

//
// Assertion override, purely for unit testing purposes.
//

void (*g_sc_assert) ();

#if defined(NDEBUG)
#   define SC_ASSERT(expr)  SC_ASSUME(expr)
#else // !defined(NDEBUG)
#   define SC_ASSERT(expr)                              \
        do {                                            \
            if (g_sc_assert) {                          \
                if (!(expr)) {                          \
                    g_sc_assert();                      \
                }                                       \
            } else {                                    \
                assert(expr);                           \
            }                                           \
        } while (0,0)
#endif // !defined(NDEBUG)

//
// Private implementation
//

struct sc_context {
    fcontext_t fctx;
    sc_context_proc_t proc;
};

namespace {

    thread_local sc_context t_main;
    thread_local sc_context* t_current;

    uintptr_t align_down (uintptr_t addr, uintptr_t alignment) {
        SC_ASSERT(alignment > 0);
        SC_ASSERT((alignment & (alignment - 1)) == 0);
        return addr & ~(alignment - 1);
    }

    void context_proc (transfer_t transfer) {
        auto data = (sc_context*)transfer.data;
        SC_ASSERT(data != nullptr);

        // Jump back to parent
        transfer = jump_fcontext(transfer.fctx, nullptr);

        // Update the current context
        sc_current_context()->fctx = transfer.fctx;
        t_current = data;

        // Execute the context proc
        data->proc(transfer.data);

        // TODO: If a proc gets to run to its end we should exit the thread, not the process.
        exit(0);
    }

} // namespace

//
// Public implementation
//

extern "C" sc_context_t SC_CALL_DECL sc_context_create (
    void* stack_ptr,
    size_t stack_size,
    sc_context_proc_t proc
) {
    SC_ASSERT(stack_ptr != nullptr);
    SC_ASSERT(stack_size >= SC_MIN_STACK_SIZE);
    SC_ASSERT(proc != nullptr);

    // Determine the bottom of the stack
    auto stack_addr = (uintptr_t)stack_ptr;
    auto sp_addr = stack_addr + stack_size;

    // Reserve some space at the bottom for the context data
    auto data_addr = sp_addr - sizeof(sc_context);
    data_addr = align_down(data_addr, alignof(sc_context));
    SC_ASSERT(data_addr > stack_addr);
    sp_addr = data_addr;

    // Align the stack pointer to a 64-byte boundary
    sp_addr = align_down(sp_addr, 64);
    SC_ASSERT(sp_addr > stack_addr);

    // Determine the new stack size
    stack_size = sp_addr - stack_addr;

    // Create the context
    auto fctx = make_fcontext((void*)sp_addr, stack_size, context_proc);
    SC_ASSERT(fctx != nullptr);

    // Create the context data at the reserved address
    auto data = new((void*)data_addr) sc_context;
    data->proc = proc;

    // Transfer the proc pointer to the context by briefly switching to it
    data->fctx = jump_fcontext(fctx, data).fctx;
    return data;
}

extern "C" void SC_CALL_DECL sc_context_destroy (sc_context_t context) {
    SC_ASSERT(context != sc_current_context());
    SC_ASSERT(context != sc_main_context());
    context->~sc_context();
}

extern "C" void* SC_CALL_DECL sc_yield (sc_context_t target, void* value) {
    SC_ASSERT(target != nullptr);

    auto this_ctx = sc_current_context();

    if (target != this_ctx) {
        auto transfer = jump_fcontext(target->fctx, value);
        sc_current_context()->fctx = transfer.fctx;
        t_current = this_ctx;
        value = transfer.data;
    }

    return value;

}

extern "C" sc_context_t SC_CALL_DECL sc_current_context () {
    return t_current ? t_current : &t_main;
}

extern "C" sc_context_t SC_CALL_DECL sc_main_context () {
    return &t_main;
}
