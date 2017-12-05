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

#endif
