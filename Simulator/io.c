#include "io.h"
#include <string.h>
#include <stdio.h>

// Initialize all I/O registers to 0
void init_io(IORegisters *io) {
	memset(io->IORegister, 0, sizeof(io->IORegister));
}

// Read a value from an I/O register
uint32_t io_read(const IORegisters *io, int reg_index) {
	if (reg_index < 0 || reg_index >= NUM_IO_REGISTERS) {
		printf("Error: Invalid I/O register index %d\n", reg_index);
		return 0;
	}
	return io->IORegister[reg_index];
}

// Write a value to an I/O register, respecting its bit width
void io_write(IORegisters *io, int reg_index, uint32_t value) {
	if (reg_index < 0 || reg_index >= NUM_IO_REGISTERS) {
		printf("Error: Invalid I/O register index %d\n", reg_index);
		return;
	}

	int bit_width = IO_REGISTER_SIZES[reg_index];
	if (bit_width > 0) {
		uint32_t mask = (bit_width == 32) ? 0xFFFFFFFF : (1U << bit_width) - 1;
		io->IORegister[reg_index] = value & mask;
	}
}

// Increment the clock counter
void io_increment_clock(IORegisters *io) {
	io_write(io, 8, io->IORegister[8] + 1); // Increment clks register
}

// Update the timer registers
void update_timer(IORegisters *io) {
	if (io->IORegister[11]) { // timerenable
		if (io->IORegister[12] > 0) {
			io->IORegister[12]--; // Decrement timercurrent
		}
		else {
			io->IORegister[12] = io->IORegister[13]; // Reset to timermax
			io->IORegister[3] = 1; // Set irq0status
		}
	}
}

