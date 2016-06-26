/*
            Copyright Oliver Kowalke 2009.
            Copyright Thomas Sailer 2013.
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
            http://www.boost.org/LICENSE_1_0.txt)
*/

/*
   Updated by Johan Sköld for sc (https://github.com/rhoot/sc)

   - 2016: XMM6-XMM15 must be preserved by the callee in Windows x64.
*/

/**************************************************************************************
 * ---------------------------------------------------------------------------------- *
 * |    0x0  |    0x4  |    0x8   |    0xc  |   0x10  |   0x14  |   0x18  |   0x1c  | *
 * ---------------------------------------------------------------------------------- *
 * |                 XMM15                  |                 XMM14                 | *
 * ---------------------------------------------------------------------------------- *
 * ---------------------------------------------------------------------------------- *
 * |   0x20  |   0x24  |   0x28   |   0x2c  |   0x30  |   0x34  |   0x38  |   0x3c  | *
 * ---------------------------------------------------------------------------------- *
 * |                 XMM13                  |                 XMM12                 | *
 * ---------------------------------------------------------------------------------- *
 * ---------------------------------------------------------------------------------- *
 * |   0x40  |   0x44  |   0x48   |   0x4c  |   0x50  |   0x54  |   0x58  |   0x5c  | *
 * ---------------------------------------------------------------------------------- *
 * |                 XMM11                  |                 XMM10                 | *
 * ---------------------------------------------------------------------------------- *
 * ---------------------------------------------------------------------------------- *
 * |   0x60  |   0x64  |   0x68   |   0x6c  |   0x70  |   0x74  |   0x78  |   0x7c  | *
 * ---------------------------------------------------------------------------------- *
 * |                 XMM9                   |                 XMM8                  | *
 * ---------------------------------------------------------------------------------- *
 * ---------------------------------------------------------------------------------- *
 * |   0x80  |   0x84  |   0x88   |   0x8c  |   0x90  |   0x94  |   0x98  |   0x9c  | *
 * ---------------------------------------------------------------------------------- *
 * |                 XMM7                   |                 XMM6                  | *
 * ---------------------------------------------------------------------------------- *
 * ---------------------------------------------------------------------------------- *
 * |   0xa0  |   0xa4  |   0xa8   |   0xac  |   0xb0  |   0xb4  |   0xb8  |   0xbc  | *
 * ---------------------------------------------------------------------------------- *
 * |       align       |      fbr_strg      |     fc_dealloc    |       limit       | *
 * ---------------------------------------------------------------------------------- *
 * ---------------------------------------------------------------------------------- *
 * |   0xc0  |  0xc4   |   0xc8   |   0xcc  |   0xd0  |   0xd4  |   0xd8  |   0xdc  | *
 * ---------------------------------------------------------------------------------- *
 * |        base       |         R12        |        R13        |        R14        | *
 * ---------------------------------------------------------------------------------- *
 * ---------------------------------------------------------------------------------- *
 * |   0xe0  |  0xe4   |   0xe8   |   0xec  |   0xf0  |   0xf4  |   0xf8  |   0xfc  | *
 * ---------------------------------------------------------------------------------- *
 * |        R15        |        RDI         |        RSI        |        RBX        | *
 * ---------------------------------------------------------------------------------- *
 * ---------------------------------------------------------------------------------- *
 * |  0x100  |  0x104  |  0x108   |  0x10c  |  0x110  |  0x114  |  0x118  |  0x11c  | *
 * ---------------------------------------------------------------------------------- *
 * |        RBP        |       hidden       |        RIP        |        EXIT       | *
 * ---------------------------------------------------------------------------------- *
 * ---------------------------------------------------------------------------------- *
 * |  0x120  |  0x124  |  0x128   |  0x12c  |  0x130  |  0x134  |  0x138  |  0x13c  | *
 * ---------------------------------------------------------------------------------- *
 * |                                 parameter area                                 | *
 * ---------------------------------------------------------------------------------- *
 * ---------------------------------------------------------------------------------- *
 * |  0x140  |  0x144  |  0x148   |  0x14c  |  0x150  |  0x154  |  0x158  |  0x15c  | *
 * ---------------------------------------------------------------------------------- *
 * |       FCTX        |         DATA       |                   |                   | *
 * ---------------------------------------------------------------------------------- *
 *                                                                                    *
 * ***********************************************************************************/

.file	"make_x86_64_ms_pe_gas.S"
.text
.p2align 4,,15
.globl	make_fcontext
.def	make_fcontext;	.scl	2;	.type	32;	.endef
.seh_proc	make_fcontext
make_fcontext:
.seh_endprologue

    /* first arg of make_fcontext() == top of context-stack */
    movq  %rcx, %rax

    /* shift address in RAX to lower 16 byte boundary */
    /* == pointer to fcontext_t and address of context stack */
    andq  $-16, %rax

    /* reserve space for context-data on context-stack */
    /* EXIT will be used as the return address for the context-function and */
    /* must have its end be 16-byte aligned */

    /* 160 bytes xmm storage, 8 bytes alignment, 168 bytes stack data */
    leaq  -0x150(%rax), %rax
    movq  $0xa8, %r9

    /* third arg of make_fcontext() == address of context-function */
    movq  %r8, 0x68(%rax, %r9)

    /* first arg of make_fcontext() == top of context-stack */
    /* save top address of context stack as 'base' */
    movq  %rcx, 0x18(%rax, %r9)
    /* second arg of make_fcontext() == size of context-stack */
    /* negate stack size for LEA instruction (== substraction) */
    negq  %rdx
    /* compute bottom address of context stack (limit) */
    leaq  (%rcx,%rdx), %rcx
    /* save bottom address of context stack as 'limit' */
    movq  %rcx, 0x10(%rax, %r9)
    /* save address of context stack limit as 'dealloction stack' */
    movq  %rcx, 0x8(%rax, %r9)
	/* set fiber-storage to zero */
    xorq  %rcx, %rcx
    movq  %rcx, (%rax, %r9)

    /* compute address of transport_t */
    leaq  0x98(%rax, %r9), %rcx
    /* store address of transport_t in hidden field */
    movq %rcx, 0x60(%rax, %r9)

    /* compute abs address of label finish */
    leaq  finish(%rip), %rcx
    /* save address of finish as return-address for context-function */
    /* will be entered after context-function returns */
    movq  %rcx, 0x70(%rax, %r9)

    ret /* return pointer to context-data */

finish:
    /* 32byte shadow-space for _exit() */
    andq  $-32, %rsp
    /* 32byte shadow-space for _exit() are */
    /* already reserved by make_fcontext() */
    /* exit code is zero */
    xorq  %rcx, %rcx
    /* exit application */
    call  _exit
    hlt
.seh_endproc

.def	_exit;	.scl	2;	.type	32;	.endef  /* standard C library function */

.section .drectve
.ascii " -export:\"make_fcontext\""
