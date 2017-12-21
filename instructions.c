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
	uint8_t reg = get_code8(cpu, 0) - 0xb8;
	uint32_t value = get_code32(cpu, 1);
	cpu->reg_array[reg] = value;
	cpu->eip += 5;
}

void inst_short_jmp(cpu_t *cpu) {
	int8_t rel_addr = get_sign_code8(cpu, 1);
	cpu->eip += 2 + rel_addr;
}

void inst_near_jmp(cpu_t *cpu) {
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

uint8_t get_mem8(cpu_t *cpu, uint32_t addr) {
	return cpu->memory[addr];
}

uint32_t get_mem32(cpu_t *cpu, uint32_t addr) {
	uint32_t value = 0;
	for (int i = 0; i < 4; i++) {
		value |= get_mem8(cpu, addr) << (i * 8);
		addr++;
	}
	return value;
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

uint32_t get_rm32(cpu_t *cpu, modrm_sib_disp_t *msd) {
	if (msd->mod == 3) {
		return get_reg32(cpu, msd);
	} else {
		uint32_t addr = calc_addr(cpu, msd);
		return get_mem32(cpu, addr);
	}
}

void set_rm32(cpu_t *cpu, modrm_sib_disp_t *msd, uint32_t value) {
	if (msd->mod == 3) {
		set_reg32(cpu, msd, value);
	} else {
		uint32_t addr = calc_addr(cpu, msd);
		set_mem32(cpu, addr, value);
	}
}

uint32_t get_r32(cpu_t *cpu, modrm_sib_disp_t *msd) {
	return cpu->reg_array[msd->reg];
}

void set_r32(cpu_t *cpu, modrm_sib_disp_t *msd, uint32_t value) {
	cpu->reg_array[msd->reg] = value;
}

void inst_mov_rm32_imm32(cpu_t *cpu) {
	cpu->eip++;
	modrm_sib_disp_t msd;
	parse_modrm_sib_disp(cpu, &msd);
	uint32_t value = get_code32(cpu, 0);
	cpu->eip += 4;
	set_rm32(cpu, &msd, value);
}

void inst_mov_rm32_r32(cpu_t *cpu) {
	cpu->eip++;
	modrm_sib_disp_t msd;
	parse_modrm_sib_disp(cpu, &msd);
	uint32_t value = get_r32(cpu, &msd);
	set_rm32(cpu, &msd, value);
}

void inst_mov_r32_rm32(cpu_t *cpu) {
	cpu->eip++;
	modrm_sib_disp_t msd;
	parse_modrm_sib_disp(cpu, &msd);
	uint32_t value = get_rm32(cpu, &msd);
	set_r32(cpu, &msd, value);
}

void inst_add_rm32_r32(cpu_t *cpu) {
	cpu->eip++;
	modrm_sib_disp_t msd;
	parse_modrm_sib_disp(cpu, &msd);
	uint32_t rm32 = get_rm32(cpu, &msd);
	uint32_t r32 = get_r32(cpu, &msd);
	set_rm32(cpu, &msd, rm32 + r32);
}

void code_83_sub_rm32_imm8(cpu_t *cpu, modrm_sib_disp_t *msd) {
	uint32_t rm32 = get_rm32(cpu, msd);
	uint8_t imm8 = get_sign_code8(cpu, 0);
	cpu->eip++;
	set_rm32(cpu, msd, rm32 - imm8);
}

void inst_code_83(cpu_t *cpu) {
	cpu->eip++;
	modrm_sib_disp_t msd;
	parse_modrm_sib_disp(cpu, &msd);
	if (msd.reg == 5) {
		code_83_sub_rm32_imm8(cpu, &msd);
	} else {
		exit_program("inst_code_82: not implemented eip=%8X", cpu->eip);
	}
}

void code_ff_inc_rm32(cpu_t *cpu, modrm_sib_disp_t *msd) {
	uint32_t rm32 = get_rm32(cpu, msd);
	set_rm32(cpu, msd, ++rm32);
}

void code_ff_dec_rm32(cpu_t *cpu, modrm_sib_disp_t *msd) {
	uint32_t rm32 = get_rm32(cpu, msd);
	set_rm32(cpu, msd, --rm32);
}

void inst_code_ff(cpu_t *cpu) {
	cpu->eip++;
	modrm_sib_disp_t msd;
	parse_modrm_sib_disp(cpu, &msd);
	switch (msd.reg) {
	case 0:
		code_ff_inc_rm32(cpu, &msd);
		break;
	case 1:
		code_ff_dec_rm32(cpu, &msd);
		break;
	default:
		exit_program("inst_code_ff: not implemented REG=%X", msd.reg);
	}
}

void push(cpu_t *cpu, uint32_t value) {
	cpu->regs.esp -= 4;
	set_mem32(cpu, cpu->regs.esp, value);
}

uint32_t pop(cpu_t *cpu) {
	uint32_t value = get_mem32(cpu, cpu->regs.esp);
	cpu->regs.esp += 4;
	return value;
}

void inst_push_r32(cpu_t *cpu) {
	uint8_t reg = get_code8(cpu, 0) - 0x50;
	uint32_t value = cpu->reg_array[reg];
	push(cpu, value);
	cpu->eip++;
}

void inst_pop_r32(cpu_t *cpu) {
	uint8_t reg = get_code8(cpu, 0) - 0x58;
	cpu->reg_array[reg] = pop(cpu);
	cpu->eip++;
}

void inst_call_rel32(cpu_t *cpu) {
	uint32_t ret_addr = cpu->eip + 5;
	push(cpu, ret_addr);
	uint32_t jump_rel_addr = get_sign_code32(cpu, 1);
	// The relative address is based on the address of the next opcode
	cpu->eip += 5 + jump_rel_addr;
}

void inst_ret(cpu_t *cpu) {
	cpu->eip = pop(cpu);
}

void inst_leave(cpu_t *cpu) {
	cpu->regs.esp = cpu->regs.ebp;
	cpu->regs.ebp = pop(cpu);
	cpu->eip++;
}

void init_instructions() {
	memset(instructions, 0, sizeof(instructions));

	instructions[0x01] = inst_add_rm32_r32;

	for (int i = 0; i < 8; i++) {
		instructions[0x50 + i] = inst_push_r32;
		instructions[0x58 + i] = inst_pop_r32;
	}

	instructions[0x83] = inst_code_83;

	instructions[0x89] = inst_mov_rm32_r32;
	instructions[0x8B] = inst_mov_r32_rm32;

	for (int i = 0; i < 8; i++) {
		instructions[0xb8 + i] = inst_mov_r32_imm32;
	}

	instructions[0xc7] = inst_mov_rm32_imm32;
	instructions[0xc3] = inst_ret;
	instructions[0xc9] = inst_leave;
	instructions[0xe8] = inst_call_rel32;
	instructions[0xe9] = inst_near_jmp;
	instructions[0xeb] = inst_short_jmp;

	instructions[0xff] = inst_code_ff;
}
