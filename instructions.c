#include <stdio.h>
#include <string.h>

#include "instructions.h"
#include "utils.h"

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

void parse_modrm_sib_disp(cpu_t *cpu, modrm_sib_disp_t *msd) {
	memset(msd, 0, sizeof(modrm_sib_disp_t));

	uint8_t code = get_code8(cpu, 0);
	msd->mod = (code & 0xC0) >> 6;
	msd->reg = (code & 0x38) >> 3;
	msd->rm = code & 0x07;

	cpu->eip++;

	// SIB
	if (msd->mod != 3 && msd->rm == 4) {
		msd->sib = get_code8(cpu, 0);
		cpu->eip++;
	}

	// disp8 or disp32
	if (msd->mod == 1) {
		msd->disp8 = get_sign_code8(cpu, 0);
		cpu->eip++;
	} else if ((msd->mod == 0 && msd->rm == 5) || msd->mod == 2) {
		msd->disp32 = get_sign_code32(cpu, 0);
		cpu->eip += 4;
	}
}

void set_reg32(cpu_t *cpu, modrm_sib_disp_t *msd, uint32_t value) {
	cpu->reg_array[msd->rm] = value;
}

uint32_t get_reg32(cpu_t *cpu, modrm_sib_disp_t *msd) {
	return cpu->reg_array[msd->rm];
}

void set_mem8(cpu_t *cpu, uint32_t addr, uint8_t value) {
	cpu->memory[addr] = value;
}

void set_mem32(cpu_t *cpu, uint32_t addr, uint32_t value) {
	for (int i = 0; i < 4; i++) {
		set_mem8(cpu, addr + i, value >> (i * 8));
	}
}

uint32_t calc_addr(cpu_t *cpu, modrm_sib_disp_t *msd) {
	switch (msd->mod) {
	case 0:
		switch (msd->rm) {
		case 4: // [-][-]
			exit_program("calc_addr: not implemented Mod=00 R/M=100");
			break;
		case 5: // disp32
			return msd->disp32;
		default:
			return get_reg32(cpu, msd);
		}
	case 1:
		// [-][-]
		if (msd->rm == 4) {
			exit_program("calc_addr: not implemented Mod=01 R/M=100");
		}
		return get_reg32(cpu, msd) + msd->disp8;
	case 2:
		// [-][-]
		if (msd->rm == 4) {
			exit_program("calc_addr: not implemented Mod=02 R/M=100");
		}
		return get_reg32(cpu, msd) + msd->disp32;
	default:
		exit_program("calc_addr: not implemented");
	}
	exit_program("calc_addr: BUG?");
	return 0;
}

void set_rm32(cpu_t *cpu, modrm_sib_disp_t *msd, uint32_t value) {
	if (msd->mod == 3) {
		set_reg32(cpu, msd, value);
	} else {
		uint32_t addr = calc_addr(cpu, msd);
		set_mem32(cpu, addr, value);
	}
}

void inst_mov_rm32_imm32(cpu_t *cpu) {
	cpu->eip++;
	modrm_sib_disp_t msd;
	parse_modrm_sib_disp(cpu, &msd);
	uint32_t value = get_code32(cpu, 0);
	cpu->eip += 4;
	set_rm32(cpu, &msd, value);
}

void init_instructions() {
	memset(instructions, 0, sizeof(instructions));
	for (int i = 0; i < 8; i++) {
		instructions[0xb8 + i] = inst_mov_r32_imm32;
	}

	instructions[0xc7] = inst_mov_rm32_imm32;
	instructions[0xe9] = inst_near_jmp;
	instructions[0xeb] = inst_short_jmp;
}
