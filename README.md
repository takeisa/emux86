# X86 Emulator in C

## Example of compiling and execution
```
$ nasm -g -o hello_world.bin hello_world.asm
$ ndisasm -b 32 hello_world.bin
00000000  B829000000        mov eax,0x29
00000005  C7C104030201      mov ecx,0x1020304
0000000B  C7C000100000      mov eax,0x1000
00000011  C70005040302      mov dword [eax],0x2030405
00000017  C705041000000605  mov dword [dword 0x1004],0x3040506
         -0403
00000021  C7400807060504    mov dword [eax+0x8],0x4050607
00000028  BB00200000        mov ebx,0x2000
0000002D  C78310F0FFFF0807  mov dword [ebx-0xff0],0x5060708
         -0605
00000037  E9C483FFFF        jmp dword 0xffff8400
$ make
gcc -c main.c -g -O0 -Wall -o main.o
gcc -c utils.c -g -O0 -Wall -o utils.o
gcc -c cpu.c -g -O0 -Wall -o cpu.o
gcc -c instructions.c -g -O0 -Wall -o instructions.o
gcc main.o utils.o cpu.o instructions.o -o emux86
$ ./emux86 hello_world.bin
mov_r_32_imm32
near_jmp
End of program
== Registers ==
EAX=00001000
ECX=01020304
EDX=00000000
EBX=00002000
ESP=00007C00
EBP=00000000
ESI=00000000
EDI=00000000
EIP=00000000
== Memory==
1000 05 04 03 02 06 05 04 03
1008 07 06 05 04 00 00 00 00
1010 08 07 06 05 00 00 00 00
1018 00 00 00 00 00 00 00 00
```
