    BITS 32
    ORG 0x7c00

start:
    mov     eax, 0x1000
    mov     dword [eax], 0x01020304
    sub     dword [eax], 0x02
    jmp     0
