    BITS 32
    ORG 0x7c00

start:
    mov     eax, 0x01020304
    push    eax
    pop     ebx
    jmp     0
