#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "cpu.h"

typedef void instruction_func_t(cpu_t *);

extern instruction_func_t *instructions[0x100];

void init_instructions();
uint8_t get_code8(cpu_t *cpu, int index);
int8_t get_sign_code8(cpu_t *cpu, int index);
uint32_t get_code32(cpu_t *cpu, int index);
int32_t get_sign_code32(cpu_t *cpu, int index);

typedef struct {
	uint8_t mod;
	uint8_t reg;
	uint8_t rm;
	uint8_t sib;
	union {
		uint8_t disp8;
		uint32_t disp32;
	};
} modrm_sib_disp_t;

void parse_modrm_sib_disp(cpu_t *cpu, modrm_sib_disp_t *msd);

#endif
