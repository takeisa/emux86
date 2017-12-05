#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdint.h>

#define REGISTER_COUNT 8

extern const char *register_names[];

typedef struct {
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
} registers_t;

typedef struct {
	union {
		uint32_t reg_array[REGISTER_COUNT];
		registers_t regs;
	};
	uint32_t eflags;
	uint32_t eip;
	uint8_t *memory;
} cpu_t;

cpu_t *create_cpu(size_t memory_size, uint32_t eip, uint32_t esp);
void destroy_cpu(cpu_t *cpu);

#endif
