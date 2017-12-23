    BITS 32
    ORG 0x7c00

start:
    mov     eax, 0x1000
    mov     dword [eax], 0x03
    mov     ebx, 0x03
    cmp     ebx, [eax]
    jmp     0
