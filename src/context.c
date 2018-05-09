/*
 * Copyright (c) 2018 Johan Sköld
 * License: https://opensource.org/licenses/ISC
 */

#include "context.h"

//
// Windows x86
//

#if defined(_WIN32) && (defined(_M_IX86) || defined(_X86_))

SC_EXTERN void SC_CALL_DECL sc_context_state (sc_state_t* state, sc_context_sp_t ctx) {
    state->type = SC_CPU_TYPE_X86;

    if (ctx) {
        uint32_t* stack = (uint32_t*)ctx;
        state->registers.x86.edi = stack[5];
        state->registers.x86.esi = stack[6];
        state->registers.x86.ebx = stack[7];
        state->registers.x86.ebp = stack[8];
        state->registers.x86.eip = stack[9];
        state->registers.x86.esp = (uint32_t)&stack[10];
    } else {
#if defined(__MINGW32__)
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
#else
        state->registers.x86.eip = _ReturnAddress();
        __asm {
            mov ecx, state
            mov [ecx]sc_state_t.registers.x86.edi, edi
            mov [ecx]sc_state_t.registers.x86.esi, esi
            mov [ecx]sc_state_t.registers.x86.ebx, ebx
            mov [ecx]sc_state_t.registers.x86.ebp, ebp
            mov [ecx]sc_state_t.registers.x86.esp, esp
        }
#endif
    }
}

#endif


//
// Windows x64
//

#if defined(_WIN64)

#if !defined(__MINGW64__)
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#endif

SC_EXTERN void SC_CALL_DECL sc_context_state (sc_state_t* state, sc_context_sp_t ctx) {
    state->type = SC_CPU_TYPE_X64;

    if (ctx) {
        uint64_t* stack = (uint64_t*)ctx;
        state->registers.x64.r12 = stack[25];
        state->registers.x64.r13 = stack[26];
        state->registers.x64.r14 = stack[27];
        state->registers.x64.r15 = stack[28];
        state->registers.x64.rdi = stack[29];
        state->registers.x64.rsi = stack[30];
        state->registers.x64.rbx = stack[31];
        state->registers.x64.rbp = stack[32];
        state->registers.x64.rip = stack[34];
        state->registers.x64.rsp = (uint64_t)&stack[35];
    } else {
#if defined(__MINGW64__)
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
#else
        CONTEXT regs;
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
#endif
    }
}

#endif


//
// macOS (x86)
//

#if defined(__APPLE__) && defined(__i386__)

SC_EXTERN void SC_CALL_DECL sc_context_state (sc_state_t* state, sc_context_sp_t ctx) {
    state->type = SC_CPU_TYPE_X86;

    if (ctx) {
        uint32_t* stack = (uint32_t*)ctx;
        state->registers.x86.edi = stack[0];
        state->registers.x86.esi = stack[1];
        state->registers.x86.ebx = stack[2];
        state->registers.x86.ebp = stack[3];
        state->registers.x86.eip = stack[4];
        state->registers.x86.esp = (uint32_t)&stack[5];
    } else {
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
    }
}

#endif


//
// macOS (x64)
//

#if defined(__APPLE__) && defined(__x86_64__)

SC_EXTERN void SC_CALL_DECL sc_context_state (sc_state_t* state, sc_context_sp_t ctx) {
    state->type = SC_CPU_TYPE_X64;

    if (ctx) {
        uint64_t* stack = (uint64_t*)ctx;
        state->registers.x64.r12 = stack[0];
        state->registers.x64.r13 = stack[1];
        state->registers.x64.r14 = stack[2];
        state->registers.x64.r15 = stack[3];
        state->registers.x64.rdi = 0;
        state->registers.x64.rsi = 0;
        state->registers.x64.rbx = stack[4];
        state->registers.x64.rbp = stack[5];
        state->registers.x64.rip = stack[6];
        state->registers.x64.rsp = (uint64_t)&stack[7];
    } else {
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
    }
}

#endif
