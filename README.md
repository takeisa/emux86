# X86 Emulator in C

## Example of Compiling and Excecution

### Compiling (Emulator)
```
$ make
gcc -c main.c -g -O0 -Wall -o main.o
gcc -c utils.c -g -O0 -Wall -o utils.o
gcc -c cpu.c -g -O0 -Wall -o cpu.o
gcc -c instructions.c -g -O0 -Wall -o instructions.o
gcc main.o utils.o cpu.o instructions.o -o emux86
```

### Sample program
```
$ cd test/c_funcs/c_func_4
$ cat crt0.asm
    BITS 32
    extern main
    global start
start:
    call main
    jmp 0

$ cat test_func.c
int my_abs(int value) {
	if (value >= 0) {
		return value;
	} else {
		return -value;
	}
}

int main(void) {
	int value = -3;
	return my_abs(value);
}
```

### Compling (C sample code)
```
$ LANG=C gcc -O0 -m32 -nostdlib -fno-pie -fno-asynchronous-unwind-tables -g -fno-stack-protector -c test_func.c
$ nasm -f elf test/c_funcs/c_func_4/crt0.asm
$ ld -m elf_i386 --entry=start --oformat=binary -Ttext 0x7c00 -o test_func.bin crt0.o test_func.o
$ ndisasm -b 32 test_func.bin
00000000  E81A000000        call dword 0x1f
00000005  E9F683FFFF        jmp dword 0xffff8400
0000000A  55                push ebp
0000000B  89E5              mov ebp,esp
0000000D  837D0800          cmp dword [ebp+0x8],byte +0x0
00000011  7805              js 0x18
00000013  8B4508            mov eax,[ebp+0x8]
00000016  EB05              jmp short 0x1d
00000018  8B4508            mov eax,[ebp+0x8]
0000001B  F7D8              neg eax
0000001D  5D                pop ebp
0000001E  C3                ret
0000001F  55                push ebp
00000020  89E5              mov ebp,esp
00000022  83EC10            sub esp,byte +0x10
00000025  C745FCFDFFFFFF    mov dword [ebp-0x4],0xfffffffd
0000002C  FF75FC            push dword [ebp-0x4]
0000002F  E8D6FFFFFF        call dword 0xa
00000034  83C404            add esp,byte +0x4
00000037  C9                leave
00000038  C3                ret
```

### Execution
```
$ cd ../../..
$ ./emux86 test/c_funcs/c_func_4/test_func.bin
End of program
== Registers ==
EAX=00000003
ECX=00000000
EDX=00000000
EBX=00000000
ESP=00007C00
EBP=00000000
ESI=00000000
EDI=00000000
EIP=00000000
ELAGS=00000001
  OF=0 SF=0 ZF=0 CF=1
== Memory==
1000 00 00 00 00 00 00 00 00
1008 00 00 00 00 00 00 00 00
1010 00 00 00 00 00 00 00 00
1018 00 00 00 00 00 00 00 00
== Memory(Stack)==
7BE0 34 7C 00 00 FD FF FF FF
7BE8 00 00 00 00 00 00 00 00
7BF0 00 00 00 00 00 00 00 00
7BF8 00 00 00 00 05 7C 00 00
```