; ---
;  Copyright (c) 2016 Johan Sköld
;  License: https://opensource.org/licenses/ISC
; ---

.code

set_xmm_register PROC FRAME
    .endprolog

    ; First argument (rcx) = register to set
    ; Second argument (xmm0) = value to set it to

    cmp  rcx,  6
    je  set_6
    cmp  rcx,  7
    je  set_7
    cmp  rcx,  8
    je  set_8
    cmp  rcx,  9
    je  set_9
    cmp  rcx,  10
    je  set_10
    cmp  rcx,  11
    je  set_11
    cmp  rcx,  12
    je  set_12
    cmp  rcx,  13
    je  set_13
    cmp  rcx,  14
    je  set_14
    jmp  set_15

set_6:
    movdqa  xmm6,  xmm0
    ret

set_7:
    movdqa  xmm7,  xmm0
    ret

set_8:
    movdqa  xmm8,  xmm0
    ret

set_9:
    movdqa  xmm9,  xmm0
    ret

set_10:
    movdqa  xmm10,  xmm0
    ret

set_11:
    movdqa  xmm11,  xmm0
    ret

set_12:
    movdqa  xmm12,  xmm0
    ret

set_13:
    movdqa  xmm13,  xmm0
    ret

set_14:
    movdqa  xmm14,  xmm0
    ret

set_15:
    movdqa  xmm15,  xmm0
    ret

set_xmm_register ENDP

END
