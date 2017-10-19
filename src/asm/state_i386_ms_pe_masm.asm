; Copyright (c) 2017 Johan Sköld
; License: https://opensource.org/licenses/ISC
; 

;  ---------------------------------------------------------------------------------
;  |    0    |    1    |    2    |    3    |    4    |    5    |    6    |    7    |
;  ---------------------------------------------------------------------------------
;  |    0h   |   04h   |   08h   |   0ch   |   010h  |   014h  |   018h  |   01ch  |
;  ---------------------------------------------------------------------------------
;  | fc_strg |fc_deallo|  limit  |   base  |  fc_seh |   EDI   |   ESI   |   EBX   |
;  ---------------------------------------------------------------------------------
;  ---------------------------------------------------------------------------------
;  |    8    |    9    |   10    |    11   |    12   |    13   |    14   |    15   |
;  ---------------------------------------------------------------------------------
;  |   020h  |  024h   |  028h   |   02ch  |   030h  |   034h  |   038h  |   03ch  |
;  ---------------------------------------------------------------------------------
;  |   EBP   |   EIP   |    to   |   data  |         |  EH NXT |SEH HNDLR|         |
;  ---------------------------------------------------------------------------------


.386
.XMM
.model flat, c
.code

sc_context_state PROC

    ; first arg : sc_state_t to populate
    mov     ecx, [esp + 04h]

    ; second arg : context to populate from
    mov     edx, [esp + 08h]

    ; set the CPU type to SC_CPU_TYPE_X86
    mov     eax, 1
    mov     [ecx], eax

    ; if the context is NULL we want the current context
    test    edx, edx
    jz      current_ctx
    
    ; save edi/esi so we can restore them later (non-volatile)
    push    edi
    push    esi

    ; copy the context's edi, esi, ebx, ebp, and eip
    lea     edi, [ecx + 008h]
    mov     ecx, 5
    lea     esi, [edx + 014h]
    cld
    rep movsd

    ; copy the context's esp, as it will be when we jump back
    lea     esi, [edx + 028h]
    mov     [edi], esi

    ; restore edi/esi
    pop     esi
    pop     edi

    ret

current_ctx:

    ; populate the state from the current context
    mov     [ecx + 008h], edi
    mov     [ecx + 00ch], esi
    mov     [ecx + 010h], ebx
    mov     [ecx + 014h], ebp

    ; eip and esp need special handling since we don't want to include this
    ; function
    mov     eax, [esp]
    lea     edx, [esp + 04h]
    mov     [ecx + 018h], eax ; eip
    mov     [ecx + 01ch], edx ; esp

    ret

sc_context_state ENDP
