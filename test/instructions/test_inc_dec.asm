    BITS 32
    ORG 0x7c00

start:
    mov     eax, 0x1000
    mov     dword [eax], 0x01020304
    inc     dword [eax]
    mov     ebx, 0x1000
    mov     dword [ebx+4], 0x02030405
    dec     dword [ebx+4]
    jmp     0
