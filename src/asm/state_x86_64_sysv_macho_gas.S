/*
 * Copyright (c) 2018 Johan Sköld
 * License: https://opensource.org/licenses/ISC
 */

/****************************************************************************************
 *                                                                                      *
 *  ----------------------------------------------------------------------------------  *
 *  |    0    |    1    |    2    |    3    |    4     |    5    |    6    |    7    |  *
 *  ----------------------------------------------------------------------------------  *
 *  |   0x0   |   0x4   |   0x8   |   0xc   |   0x10   |   0x14  |   0x18  |   0x1c  |  *
 *  ----------------------------------------------------------------------------------  *
 *  |        R12        |         R13       |         R14        |        R15        |  *
 *  ----------------------------------------------------------------------------------  *
 *  ----------------------------------------------------------------------------------  *
 *  |    8    |    9    |   10    |   11    |    12    |    13   |    14   |    15   |  *
 *  ----------------------------------------------------------------------------------  *
 *  |   0x20  |   0x24  |   0x28  |  0x2c   |   0x30   |   0x34  |   0x38  |   0x3c  |  *
 *  ----------------------------------------------------------------------------------  *
 *  |        RBX        |         RBP       |         RIP        |       EXIT        |  *
 *  ----------------------------------------------------------------------------------  *
 *                                                                                      *
 ****************************************************************************************/

#ifdef SC_META_INCLUDED

.text
.globl _sc_context_state
.align 8
_sc_context_state:

    /* first arg : sc_state_t to populate (rdi) */
    /* second arg : context to populate from (rsi) */

    /* set the CPU type to SC_CPU_TYPE_X64 */
    movq    $2, %r8
    movq    %r8, (%rdi)

    /* if the context is NULL we want the current context */
    test    %rsi, %rsi
    jz      current_ctx

    /* rsp */
    movq    %rsi, 0x50(%rdi)

    /* r12, r13, r14, r15 */
    addq    $8, %rdi
    movq    $4, %rcx
    cld
    rep movsq

    /* rdi/rsi */
    xorq    %rax, %rax
    movq    %rax, 0x00(%rdi)
    movq    %rax, 0x08(%rdi)

    /* rbx */
    movq    (%rsi), %r8
    movq    %r8, 0x10(%rdi)

    /* rbp */
    movq    0x08(%rsi), %r9
    movq    %r9, 0x18(%rdi)

    /* rip */
    movq    0x10(%rsi), %r10
    movq    %r10, 0x20(%rdi)

    ret

current_ctx:

    /* populate the state from the current state */
    movq    %r12, 0x08(%rdi)
    movq    %r13, 0x10(%rdi)
    movq    %r14, 0x18(%rdi)
    movq    %r15, 0x20(%rdi)
    movq    %rdi, 0x28(%rdi)
    movq    %rsi, 0x30(%rdi)
    movq    %rbx, 0x38(%rdi)
    movq    %rbp, 0x40(%rdi)

    /* rip and rsp need special handling since we don't want to include this function */
    movq    (%rsp), %r8
    leaq    0x08(%rsp), %r9
    movq    %r8, 0x48(%rdi) /* rip */
    movq    %r9, 0x50(%rdi) /* rsp */

    ret

#endif /* SC_META_INCLUDED */

