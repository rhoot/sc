/*
            Copyright Oliver Kowalke 2009.
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

/*****************************************************************************************
 *                                                                                       *
 *  -----------------------------------------------------------------------------------  *
 *  |    0    |    1    |    2    |    3    |    4     |    5    |    6     |    7    |  *
 *  -----------------------------------------------------------------------------------  *
 *  |   0x0   |   0x4   |   0x8   |   0xc   |   0x10   |   0x14  |   0x18   |   0x1c  |  *
 *  -----------------------------------------------------------------------------------  *
 *  |   EDI   |   ESI   |   EBX   |   EBP   |   EIP    |  hidden |    to    |   data  |  *
 *  -----------------------------------------------------------------------------------  *
 *                                                                                       *
 *****************************************************************************************/

.text
.globl _jump_fcontext
.align 2
_jump_fcontext:
    pushl  %ebp  /* save EBP */
    pushl  %ebx  /* save EBX */
    pushl  %esi  /* save ESI */
    pushl  %edi  /* save EDI */

    /* The IA-32 ABI causes 8 byte structs to be returned split between EAX
       and EDX. For us, this means the parent fcontext_t should be in EAX and
       the data in EDX. */

    /* store current fcontext_t in EAX */
    movl  %esp, %eax

    /* first arg of jump_fcontext() == context jumping to */
    movl  0x14(%esp), %ecx

    /* second arg of jump_fcontext() == data to be transferred */
    movl  0x18(%esp), %edx

    /* restore ESP (pointing to context-data) from ECX */
    movl  %ecx, %esp

    popl  %edi  /* restore EDI */
    popl  %esi  /* restore ESI */
    popl  %ebx  /* restore EBX */
    popl  %ebp  /* restore EBP */

    /* jump to context */
    ret
