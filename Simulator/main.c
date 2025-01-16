#define _CRT_SECURE_NO_WARNINGS
// Standard Library Includes
#include <stdint.h>   // For fixed-width integer types (e.g., uint32_t, uint16_t)
#include <stdio.h>    // For input/output operations (e.g., printf, fopen)
#include <stdlib.h>   // For memory allocation and exit handling

// Simulator Includes
#include "memory.h"      
#include "io.h"         
#include "registers.h"   
#include "disk.h"        
#include "interrupts.h"  
#include "instruction_fetch.h"
#include "instruction_decode.h" 
#include "execution.h" 

 // The simulator fetch-decode-exe loop
void simulator_main_loop(Registers *registers, Memory *memory, IORegisters *io, Disk *disk, IRQ2Data *irq2) {
	uint16_t pc = 0;        // Program counter (12-bit)
	int in_isr = 0;         // ISR state (0 = not in ISR, 1 = in ISR)

	while (1) {
		// Get the current clock cycle from the clks register
		uint32_t current_cycle = io->IORegister[8];

		// Check and trigger IRQ2 based on the current clock cycle
		check_and_trigger_irq2(io, irq2, current_cycle);

		// Handle interrupts if any are pending
		handle_interrupts(io, &pc, &in_isr);

		// Manage disk operations (e.g., read/write tasks)
		manage_disk(memory, io, disk);

		// Fetch the next instruction using the 12-bit PC
		uint32_t instruction = fetch_instruction(memory, &pc);

		// Decode the fetched instruction
		Instruction* decoded;
		decode_instruction(instruction, decoded, registers);

		// Execute the decoded instruction, including updates to registers
		execute_instruction(decoded, registers, memory, io,  &pc, &in_isr);

		// Increment the clock register
		increment_clock(io);
	}
}

int main() {
	printf("Hello, Simulator!\n");
	return 0;
}
