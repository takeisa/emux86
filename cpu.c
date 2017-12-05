#include "cpu.h"
#include "utils.h"

const char *register_names[] = {
	"EAX", "ECX", "EDX", "EBX",
	"ESP", "EBP", "ESI", "EDI",
	NULL
};

cpu_t *create_cpu(size_t memory_size, uint32_t eip, uint32_t esp) {
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
