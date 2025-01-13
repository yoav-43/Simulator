#define _CRT_SECURE_NO_WARNINGS
#include "execution.h"
#include <stdio.h> 
#include <stdlib.h>


// Execute the instruction from instruction decode
void execute_instruction(const Instruction *decoded_instruction, Registers *registers, Memory *memory, uint16_t *pc) {
	uint32_t rs = get_register(registers, decoded_instruction->rs);
	uint32_t rt = get_register(registers, decoded_instruction->rt);
	uint32_t rm = get_register(registers, decoded_instruction->rm);
	uint32_t rd = get_register(registers, decoded_instruction->rd);
	uint32_t imm1 = get_register(registers, REG_IMM1);
	uint32_t imm2 = get_register(registers, REG_IMM2);
	uint32_t result = 0;

	switch (decoded_instruction->opcode) {
	
	// Arithmetic Instructions
	
	case 0: // add
		result = rs + rt + rm;
		set_register(registers, decoded_instruction->rd, result);
		(*pc)++;
		break;

	case 1: // sub
		result = rs - rt - rm;
		set_register(registers, decoded_instruction->rd, result);
		(*pc)++;
		break;

	case 2: // mac
		result = (rs * rt) + rm;
		set_register(registers, decoded_instruction->rd, result);
		(*pc)++;
		break;

	// Logical Instructions
	
	case 3: // and
		result = rs & rt & rm;
		set_register(registers, decoded_instruction->rd, result);
		(*pc)++;
		break;

	case 4: // or
		result = rs | rt | rm;
		set_register(registers, decoded_instruction->rd, result);
		(*pc)++;
		break;

	case 5: // xor
		result = rs ^ rt ^ rm;
		set_register(registers, decoded_instruction->rd, result);
		(*pc)++;
		break;

	// Shift Instructions
	
	case 6: // sll
		result = rs << rt;
		set_register(registers, decoded_instruction->rd, result);
		(*pc)++;
		break;

	case 7: // sra
		result = (int32_t)rs >> rt;
		set_register(registers, decoded_instruction->rd, result);
		(*pc)++;
		break;

	case 8: // srl
		result = rs >> rt;
		set_register(registers, decoded_instruction->rd, result);
		(*pc)++;
		break;

	// Branch Instructions
	
	case 9: // beq
		if (rs == rt) {
			*pc = rm & 0x0FFF; // Use the lower 12 bits of R[rm]
		}
		else {
			(*pc)++;
		}
		break;

	case 10: // bne
		if (rs != rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			(*pc)++;
		}
		break;

	case 11: // blt
		if ((int32_t)rs < (int32_t)rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			(*pc)++;
		}
		break;

	case 12: // bgt
		if ((int32_t)rs > (int32_t)rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			(*pc)++;
		}
		break;

	case 13: // ble
		if ((int32_t)rs <= (int32_t)rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			(*pc)++;
		}
		break;

	case 14: // bge
		if ((int32_t)rs >= (int32_t)rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			(*pc)++;
		}
		break;

	case 15: // jal
		result = *pc + 1;
		set_register(registers, decoded_instruction->rd, result); // Save return address
		*pc = rm & 0x0FFF; // Jump to the address in the lower 12 bits of R[rm]
		break;

	// Memory Access Instructions
	
	case 16: // lw
		result = read_data(memory, rs + rt) + rm;
		set_register(registers, decoded_instruction->rd, result);
		(*pc)++;
		break;

	case 17: // sw
		result = rm + rd;
		write_data(memory, rs + rt, result);
		(*pc)++;
		break;

	// I/O and Control Instructions
	
	case 18: // reti
		// PC = IORegister[7]
		break;

	case 19: // in
		// R[rd] = IORegister[R[rs] + R[rt]]
		(*pc)++;
		break;

	case 20: // out
		// IORegister[R[rs] + R[rt]] = R[rm]
		(*pc)++;
		break;

	case 21: // halt
		exit(0);

	default:
		printf("Error: Unsupported opcode %d\n", decoded_instruction->opcode);
		break;
	}
}
