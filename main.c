#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

void exit_program(const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	vfprintf(stdout, format, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}

#define MEMORY_SIZE 0x10000

#define REGISTER_COUNT 8

const char *register_names[] = {
	"EAX", "ECX", "EDX", "EBX",
	"ESP", "EBP", "ESI", "EDI",
	NULL
};

typedef uint32_t reg_t;

typedef struct {
	reg_t eax;
	reg_t ecx;
	reg_t edx;
	reg_t ebx;
	reg_t esp;
	reg_t ebp;
	reg_t esi;
	reg_t edi;
} registers_t;

typedef struct {
	union {
		reg_t reg_array[REGISTER_COUNT];
		registers_t regs;
	};
	reg_t eflags;
	reg_t eip;
	uint8_t *memory;
} cpu_t;

cpu_t *create_cpu(size_t memory_size, reg_t eip, reg_t esp) {
	cpu_t *emu = malloc(sizeof(cpu_t));
	if (!emu) {
		exit_program("create_cpu: cpu malloc error");
	}

	emu->memory = calloc(sizeof(uint8_t), memory_size);
	if (!emu->memory) {
		exit_program("create_cpu: cpu memory malloc error");
	}

	emu->eip = eip;
	emu->regs.esp = esp;

	return emu;
}

void destroy_cpu(cpu_t *cpu) {
	free(cpu->memory);
	free(cpu);
}

uint8_t get_code8(cpu_t *cpu, int index) {
	return cpu->memory[cpu->eip + index];
}

int8_t get_sign_code8(cpu_t *cpu, int index) {
	return (int8_t)get_code8(cpu, index);
}

uint32_t get_code32(cpu_t *cpu, int index) {
	uint32_t value = 0;
	for (int i = 0; i < 4; i++) {
		value |= get_code8(cpu, index + i) << (i * 8);
	}
	return value;
}

int32_t get_sign_code32(cpu_t *cpu, int index) {
	return (int32_t)get_code32(cpu, index);
}


typedef void instruction_func_t(cpu_t *);

void inst_mov_r32_imm32(cpu_t *cpu) {
	printf("mov_r_32_imm32\n");
	uint8_t reg = get_code8(cpu, 0) - 0xb8;
	uint32_t value = get_code32(cpu, 1);
	cpu->reg_array[reg] = value;
	cpu->eip += 5;
}

void inst_short_jmp(cpu_t *cpu) {
	printf("short_jmp\n");
	int8_t rel_addr = get_sign_code8(cpu, 1);
	cpu->eip += 2 + rel_addr;
}

void inst_near_jmp(cpu_t *cpu) {
	printf("near_jmp\n");
	int32_t rel_addr = get_sign_code32(cpu, 1);
	cpu->eip += (5 + rel_addr);
}

instruction_func_t *instructions[0x100];

void init_instructions() {
	memset(instructions, 0, sizeof(instructions));
	for (int i = 0; i < 8; i++) {
		instructions[0xb8 + i] = inst_mov_r32_imm32;
	}

	instructions[0xe9] = inst_near_jmp;
	instructions[0xeb] = inst_short_jmp;
}

void run(cpu_t *cpu) {
	while (true) {
		if (cpu->eip >= MEMORY_SIZE) {
			exit_program("EIP over memory size: EIP=%8X\n", cpu->eip);
		}

		uint8_t code = get_code8(cpu, 0);
		if (instructions[code] == NULL) {
			printf("Not implemented: code=%02X\n", code);
			break;
		}

		instructions[code](cpu);

		if (cpu->eip == 0) {
			printf("End of program\n");
			break;
		}
	}
}

void print_registers(cpu_t *cpu) {
	for (int i = 0; register_names[i] != NULL; i++) {
		printf("%s=%08X\n", register_names[i], cpu->reg_array[i]);
	}
	printf("EIP=%08X\n", cpu->eip);
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
	print_registers(cpu);

	destroy_cpu(cpu);
	return 0;
}
