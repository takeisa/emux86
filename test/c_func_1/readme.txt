How to compile and link

$ LANG=C gcc -O0 -m32 -nostdlib -fno-pie -fno-asynchronous-unwind-tables -g -fno-stack-protector -c test_func.c
$ nasm -f elf crt0.asm
$ ld -m elf_i386 --entry=start --oformat=binary -Ttext 0x7c00 -o test_func.bin crt0.o test_func.o
