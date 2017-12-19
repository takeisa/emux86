    BITS 32
    ORG 0x7c00

start:
    mov     eax, 0x01020304
    mov     ebx, 0x02030405
    call    add_proc
    jmp     0

add_proc:
    mov     ecx, eax
    add     ecx, ebx
    ret
