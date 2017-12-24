    BITS 32
    ORG 0x7c00

start:
    mov     eax, 0x01
    neg     eax
    jmp     0
