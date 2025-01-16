#define _CRT_SECURE_NO_WARNINGS
#include "execution.h"
#include <stdio.h> 
#include <stdlib.h>


// Execute the instruction from instruction decode
void execute_instruction(const Instruction *decoded_instruction, Registers *registers, Memory *memory, IORegisters *IORegister, uint16_t *pc, int *in_isr) {
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
		increment_pc(pc);
		break;

	case 1: // sub
		result = rs - rt - rm;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	case 2: // mac
		result = (rs * rt) + rm;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	// Logical Instructions
	
	case 3: // and
		result = rs & rt & rm;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	case 4: // or
		result = rs | rt | rm;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	case 5: // xor
		result = rs ^ rt ^ rm;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	// Shift Instructions
	
	case 6: // sll
		result = rs << rt;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	case 7: // sra
		result = (int32_t)rs >> rt;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	case 8: // srl
		result = rs >> rt;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	// Branch Instructions
	
	case 9: // beq
		if (rs == rt) {
			*pc = rm & 0x0FFF; // Use the lower 12 bits of R[rm]
		}
		else {
			increment_pc(pc);
		}
		break;

	case 10: // bne
		if (rs != rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			increment_pc(pc);
		}
		break;

	case 11: // blt
		if ((int32_t)rs < (int32_t)rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			increment_pc(pc);
		}
		break;

	case 12: // bgt
		if ((int32_t)rs > (int32_t)rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			increment_pc(pc);
		}
		break;

	case 13: // ble
		if ((int32_t)rs <= (int32_t)rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			increment_pc(pc);
		}
		break;

	case 14: // bge
		if ((int32_t)rs >= (int32_t)rt) {
			*pc = rm & 0x0FFF;
		}
		else {
			increment_pc(pc);
		}
		break;

	case 15: // jal
		set_register(registers, decoded_instruction->rd, *pc + 1); // Save return address
		*pc = rm & 0x0FFF; // Jump to the address in the lower 12 bits of R[rm]
		break;

	// Memory Access Instructions
	
	case 16: // lw
		result = read_data(memory, rs + rt) + rm;
		set_register(registers, decoded_instruction->rd, result);
		increment_pc(pc);
		break;

	case 17: // sw
		write_data(memory, rs + rt, rm + rd);
		increment_pc(pc);
		break;

	// I/O and Control Instructions
	
	case 18: // reti
		*pc = io_read(IORegister, 7);
		*in_isr = 0;                 // Mark ISR as finished
		break;

	case 19: // in
		result = io_read(IORegister, rs + rt); // Read from I/O register indexed by rs
		set_register(registers, decoded_instruction->rd, result); // Write to destination register
		increment_pc(pc);
		break;

	case 20: // out
		io_write(IORegister, rs + rt, rm); // Write to I/O register
		increment_pc(pc);
		break;

	case 21: // halt
		exit(0);

	default:
		printf("Error: Unsupported opcode %d\n", decoded_instruction->opcode);
		break;
	}
}
