/*
 * Copyright (c) 2018 Johan Sköld
 * License: https://opensource.org/licenses/ISC
 */

#include "context.h"

//
// X86
//

#if defined(_M_IX86) || defined(_X86_) || defined(__i386__)

#   define SC_HAS_CONTEXT_STATE_IMPL

SC_EXTERN void SC_CALL_DECL sc_context_state (sc_state_t* state, sc_context_sp_t ctx) {
    state->type = SC_CPU_TYPE_X86;

    if (ctx) {

#   if defined(_WIN32)
        uint32_t* stack = (uint32_t*)ctx + 5;
#   else
        uint32_t* stack = (uint32_t*)ctx;
#   endif

        state->registers.x86.edi = stack[0];
        state->registers.x86.esi = stack[1];
        state->registers.x86.ebx = stack[2];
        state->registers.x86.ebp = stack[3];
        state->registers.x86.eip = stack[4];
        state->registers.x86.esp = (uint32_t)&stack[5];
    } else {
#   if defined(_MSC_VER)
        state->registers.x86.eip = (uint32_t)_ReturnAddress();
        __asm {
            mov ecx, state
            mov [ecx]sc_state_t.registers.x86.edi, edi
            mov [ecx]sc_state_t.registers.x86.esi, esi
            mov [ecx]sc_state_t.registers.x86.ebx, ebx
            mov [ecx]sc_state_t.registers.x86.ebp, ebp
            mov [ecx]sc_state_t.registers.x86.esp, esp
        }
#   else
        state->registers.x86.eip = (uint32_t)__builtin_return_address(0);
        asm (
            "movl   %%edi, 0x04(%0) \n"
            "movl   %%esi, 0x08(%0) \n"
            "movl   %%ebx, 0x0c(%0) \n"
            "movl   %%ebp, 0x10(%0) \n"
            "movl   %%esp, 0x18(%0) \n"
            :
            : "r" (state)
        );
#   endif
    }
}

#endif


//
// X64
//

#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__) || defined(__amd64__)

#   define SC_HAS_CONTEXT_STATE_IMPL

#   if defined(_MSC_VER)
#       define WIN32_LEAN_AND_MEAN
#       include <Windows.h>
#   endif

SC_EXTERN void SC_CALL_DECL sc_context_state (sc_state_t* state, sc_context_sp_t ctx) {
    state->type = SC_CPU_TYPE_X64;

    if (ctx) {
        // Windows has a bunch of data before the registers we're looking for.
#   if defined(_WIN64)
        uint64_t* stack = (uint64_t*)ctx + 25;
#   else
        uint64_t* stack = (uint64_t*)ctx;
#   endif

        state->registers.x64.r12 = *stack++;
        state->registers.x64.r13 = *stack++;
        state->registers.x64.r14 = *stack++;
        state->registers.x64.r15 = *stack++;

        // RDI and RSI are only stored for Windows
#   if defined(_WIN64)
        state->registers.x64.rdi = *stack++;
        state->registers.x64.rsi = *stack++;
#   else
        state->registers.x64.rdi = 0;
        state->registers.x64.rsi = 0;
#   endif

        state->registers.x64.rbx = *stack++;
        state->registers.x64.rbp = *stack++;
        state->registers.x64.rip = *stack++;
        state->registers.x64.rsp = (uint64_t)stack;
    } else {
#   if defined(_MSC_VER)
        CONTEXT regs;
        regs.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL;

        GetThreadContext(GetCurrentThread(), &regs);

        state->registers.x64.r12 = regs.R12;
        state->registers.x64.r13 = regs.R13;
        state->registers.x64.r14 = regs.R14;
        state->registers.x64.r15 = regs.R15;
        state->registers.x64.rdi = regs.Rdi;
        state->registers.x64.rsi = regs.Rsi;
        state->registers.x64.rbx = regs.Rbx;
        state->registers.x64.rbp = regs.Rbp;
        state->registers.x64.rip = regs.Rip;
        state->registers.x64.rsp = regs.Rsp;
#   else
        void* x64 = &state->registers.x64;
        asm (
            "leaq   (%%rip), %%r8 \n"
            "movq   %%r12, 0x00(%0) \n"
            "movq   %%r13, 0x08(%0) \n"
            "movq   %%r14, 0x10(%0) \n"
            "movq   %%r15, 0x18(%0) \n"
            "movq   %%rdi, 0x20(%0) \n"
            "movq   %%rsi, 0x28(%0) \n"
            "movq   %%rbx, 0x30(%0) \n"
            "movq   %%rbp, 0x38(%0) \n"
            "movq   %%r8, 0x40(%0) \n"
            "movq   %%rsp, 0x48(%0) \n"
            :
            : "r" (x64)
            : "r8"
        );
#   endif
    }
}

#endif


//
// ARM
//

#if defined(__arm__)

#define SC_HAS_CONTEXT_STATE_IMPL

SC_EXTERN void SC_CALL_DECL sc_context_state (sc_state_t* state, sc_context_sp_t ctx) {
    state->type = SC_CPU_TYPE_ARM;

    if (ctx) {
        uint32_t* stack = (uint32_t*)ctx;

#if defined(__APPLE__)
        // iOS has an extra pointer stored on the stack before the registers.
        ++stack;
#endif

        state->registers.arm.v1 = stack[1];
        state->registers.arm.v2 = stack[2];
        state->registers.arm.v3 = stack[3];
        state->registers.arm.v4 = stack[4];
        state->registers.arm.v5 = stack[5];
        state->registers.arm.v6 = stack[6];
        state->registers.arm.v7 = stack[7];
        state->registers.arm.v8 = stack[8];
        state->registers.arm.sp = (uint32_t)&stack[11];
        state->registers.arm.lr = stack[9];
        state->registers.arm.pc = stack[10];
    } else {
        void* arm = &state->registers.arm;
        asm (
            "stm %0, {v1-v8,lr} \n"
            "str sp, [%0, #36] \n"
            "str pc, [%0, #40] \n"
            :
            : "r" (arm)
        );
    }
}

#endif

//
// Fallback implementation
//

#if !defined(SC_HAS_CONTEXT_STATE_IMPL)

SC_EXTERN void SC_CALL_DECL sc_context_state (sc_state_t* state, sc_context_sp_t ctx) {
    (void)ctx;
    state->type = SC_CPU_TYPE_UNKNOWN;
}

#endif
