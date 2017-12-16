Example of compiling and execution
```
$ nasm -g -o hello_world.bin hello_world.asm
$ ndisasm -b 32 hello_world.bin
00000000  B829000000        mov eax,0x29
00000005  C7C104030201      mov ecx,0x1020304
0000000B  C7C000100000      mov eax,0x1000
00000011  C70005040302      mov dword [eax],0x2030405
00000017  E9E483FFFF        jmp dword 0xffff8400
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
EBX=00000000
ESP=00007C00
EBP=00000000
ESI=00000000
EDI=00000000
EIP=00000000
== Memory==
1000 05
1001 04
1002 03
1003 02
1004 00
1005 00
1006 00
1007 00
```