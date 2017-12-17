    BITS 32
    ORG 0x7c00

start:
    mov     eax, 0x1000
    mov     dword [eax], 0x01020304
    mov     ebx, 0x02030405
    add     [eax], ebx
    jmp     0
