#include <stdio.h>
#include <string.h>

#include "instructions.h"

instruction_func_t *instructions[0x100];

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

void init_instructions() {
	memset(instructions, 0, sizeof(instructions));
	for (int i = 0; i < 8; i++) {
		instructions[0xb8 + i] = inst_mov_r32_imm32;
	}

	instructions[0xe9] = inst_near_jmp;
	instructions[0xeb] = inst_short_jmp;
}
