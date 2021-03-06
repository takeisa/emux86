#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "cpu.h"
#include "instructions.h"
#include "utils.h"

#define MEMORY_SIZE 0x10000

void run(cpu_t *cpu) {
	while (true) {
		if (cpu->eip >= MEMORY_SIZE) {
			exit_program("EIP over memory size: EIP=%8X\n", cpu->eip);
		}

		uint8_t code = get_code8(cpu, 0);
		if (instructions[code] == NULL) {
			exit_program("Not implemented: eip=%08X code=%02X\n", cpu->eip, code);
		}

		instructions[code](cpu);

		if (cpu->eip == 0) {
			printf("End of program\n");
			break;
		}
	}
}

char flag_char(int value) {
	return value ? '1' : '0';
}

void print_registers(cpu_t *cpu) {
	for (int i = 0; register_names[i] != NULL; i++) {
		printf("%s=%08X\n", register_names[i], cpu->reg_array[i]);
	}
	printf("EIP=%08X\n", cpu->eip);
	printf("ELAGS=%08X\n", cpu->eflags);
	printf("  OF=%c SF=%c ZF=%c CF=%c\n",
		   flag_char(cpu->eflags & EFLAGS_OF),
		   flag_char(cpu->eflags & EFLAGS_SF),
		   flag_char(cpu->eflags & EFLAGS_ZF),
		   flag_char(cpu->eflags & EFLAGS_CF));
}

void print_usage() {
	fprintf(stderr, "usage: emux86 filename\n");
}

int main(int argc, char *argv[]) {
	int ch;
	while ((ch = getopt(argc, argv, "h")) != EOF) {
		switch(ch) {
		case 'h':
			print_usage();
			exit(1);
			break;
		}
	}
	argc -= optind;
	argv += optind;

	/* for (int i = 0; i < argc; i++) { */
	/* 	printf("%i %s\n", i, argv[i]); */
	/* } */

	if (argc < 1) {
		print_usage();
		exit(1);
	}

	init_instructions();

	cpu_t *cpu = create_cpu(MEMORY_SIZE, 0x7c00, 0x7c00);

	FILE *fp = fopen(argv[0], "rb");
	if (fp == NULL) {
		perror("binary file open error");
		exit(EXIT_FAILURE);
	}

	fread(cpu->memory + 0x7c00, sizeof(uint8_t), 0x200, fp);
	fclose(fp);

	run(cpu);
	printf("== Registers ==\n");
	print_registers(cpu);

	// for debug
	printf("== Memory==\n");
	uint32_t addr = 0x1000;
	for (int i = 0; i < 4; i++) {
		printf("%04X", addr);
		for (int j = 0; j < 8; j++) {
			printf(" %02X", cpu->memory[addr]);
			addr++;
		}
		printf("\n");
	}
	printf("== Memory(Stack)==\n");
	addr = 0x7c00 - 0x20;
	for (int i = 0; i < 4; i++) {
		printf("%04X", addr);
		for (int j = 0; j < 8; j++) {
			printf(" %02X", cpu->memory[addr]);
			addr++;
		}
		printf("\n");
	}

	destroy_cpu(cpu);
	return 0;
}
