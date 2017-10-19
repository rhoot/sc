/*
 * Copyright (c) 2017 Johan Sköld
 * License: https://opensource.org/licenses/ISC
 */

/***************************************************************************************
 *                                                                                     *
 *  ---------------------------------------------------------------------------------  *
 *  |    0    |    1    |    2    |    3    |    4    |    5    |    6    |    7    |  *
 *  ---------------------------------------------------------------------------------  *
 *  |    0h   |   04h   |   08h   |   0ch   |   010h  |   014h  |   018h  |   01ch  |  *
 *  ---------------------------------------------------------------------------------  *
 *  | fc_strg |fc_deallo|  limit  |   base  |  fc_seh |   EDI   |   ESI   |   EBX   |  *
 *  ---------------------------------------------------------------------------------  *
 *  ---------------------------------------------------------------------------------  *
 *  |    8    |    9    |   10    |    11   |    12   |    13   |    14   |    15   |  *
 *  ---------------------------------------------------------------------------------  *
 *  |   020h  |  024h   |  028h   |   02ch  |   030h  |   034h  |   038h  |   03ch  |  *
 *  ---------------------------------------------------------------------------------  *
 *  |   EBP   |   EIP   |    to   |   data  |         |  EH NXT |SEH HNDLR|         |  *
 *  ---------------------------------------------------------------------------------  *
 *                                                                                     *
 ***************************************************************************************/

#ifdef SC_META_INCLUDED

.text
.p2align 4,,15
.globl	_sc_context_state
.def	_sc_context_state;	.scl	2;	.type	32;	.endef
_sc_context_state:

    /* first arg : sc_state_t to populate */
    movl    0x4(%esp), %ecx

    /* second arg : context to populate from */
    movl    0x8(%esp), %edx

    /* set the CPU type to SC_CPU_TYPE_X86 */
    movl    $1, %eax
    movl    %eax, (%ecx)

    /* if the context is NULL we want the current context */
    test    %edx, %edx
    jz      current_ctx
    
    /* save edi/esi so we can restore them later (non-volatile) */
    pushl   %edi
    pushl   %esi

    /* copy the context's edi, esi, ebx, ebp, and eip */
    leal    0x08(%ecx), %edi
    movl    $5, %ecx
    leal    0x14(%edx), %esi
    cld
    rep movsd

    /* copy the context's esp, as it will be when we jump back */
    leal    0x28(%edx), %esi
    movl    %esi, (%edi)

    /* restore edi/esi */
    popl    %esi
    popl    %edi

    ret

current_ctx:

    /* populate the state from the current context */
    movl    %edi, 0x08(%ecx)
    movl    %esi, 0x0c(%ecx)
    movl    %ebx, 0x10(%ecx)
    movl    %ebp, 0x14(%ecx)

    /* eip and esp need special handling since we don't want to include this function */
    movl    (%esp), %eax
    leal    0x4(%esp), %edx
    movl    %eax, 0x18(%ecx) /* eip */
    movl    %edx, 0x1c(%ecx) /* esp */

    ret

.section .drectve
.ascii " -export:\"sc_context_state\""

#endif /* SC_META_INCLUDED */
