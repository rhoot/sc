// Copyright (c) 2016 Johan Sköld
// License: https://opensource.org/licenses/ISC
//---

#include <sj.h>

#include <cassert>
#include <cstdint>
#include <memory>

#include "fcontext.hpp"

struct sj_context {
    fcontext_t fctx = nullptr;
    sj_context_proc_t proc = nullptr;
};

namespace {

    thread_local sj_context t_main;
    thread_local sj_context* t_current = &t_main;

    uintptr_t align_down (uintptr_t addr, uintptr_t alignment) {
        assert(alignment > 0);
        assert(alignment & (alignment - 1) == 0);
        return addr & ~(alignment - 1);
    }

    void context_proc (transfer_t transfer) {
        auto data = (sj_context*)transfer.data;
        assert(data != nullptr);

        // Jump back to parent
        transfer = jump_fcontext(transfer.fctx, nullptr);

        // Update the current fiber
        t_current->fctx = transfer.fctx;
        t_current = data;

        // Execute the context proc
        data->proc(transfer.data);

        // TODO: If a proc gets to run to its end we should exit the thread, not the process.
        exit(0);
    }

} // namespace

extern "C" sj_context_t sj_context_create (
    void* stack_ptr,
    size_t stack_size,
    sj_context_proc_t proc
) {
    assert(stack_ptr != nullptr);
    assert(stack_size > 0);

    // Determine the bottom of the stack
    auto stack_addr = (uintptr_t)stack_ptr;
    auto sp_addr = stack_addr + stack_size;

    // Reserve some space at the bottom for the context data
    auto data_addr = sp_addr - sizeof(sj_context);
    data_addr = align_down(data_addr, alignof(sj_context));
    assert(data_addr > stack_addr);
    sp_addr = data_addr;

    // Align the stack pointer to a 64-byte boundary
    sp_addr = align_down(sp_addr, 64);
    assert(sp_addr > stack_addr);

    // Determine the new stack size
    stack_size = sp_addr - stack_addr;

    // Create the context
    auto fctx = make_fcontext((void*)sp_addr, stack_size, context_proc);
    assert(fctx != nullptr);

    // Create the context data at the reserved address
    auto data = new((void*)data_addr) sj_context;
    data->proc = proc;

    // Transfer the proc pointer to the context by briefly switching to it
    data->fctx = jump_fcontext(fctx, proc).fctx;
    return data;
}

extern "C" void sj_context_destroy (sj_context_t context) {
    assert(context != sj_current_context());
    assert(context != sj_main_context());
    context->~sj_context();
}

extern "C" void* sj_yield (sj_context_t target, void* value) {
    auto this_ctx = t_current;

    if (target != this_ctx) {
        auto transfer = jump_fcontext(target->fctx, value);
        t_current->fctx = transfer.fctx;
        t_current = this_ctx;
        value = transfer.data;
    }

    return value;

}

extern "C" sj_context_t sj_current_context () {
    return t_current;
}

extern "C" sj_context_t sj_main_context () {
    return &t_main;
}
