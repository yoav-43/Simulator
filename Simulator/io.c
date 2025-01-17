#include "io.h"
#include <string.h>
#include <stdio.h>

// Initialize all I/O registers to 0
void init_io(IORegisters *io) {
	memset(io->IORegister, 0, sizeof(io->IORegister));
	io->halt_flag = 1;
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
void increment_clock(IORegisters *io) {
	io_write(io, 8, io->IORegister[CLKS] + 1); // Increment clks register
}

// Update the timer registers
void update_timer(IORegisters *io) {
	// Check if the timer is enabled
	if (io->IORegister[TIMERENABLE] == 1) { // timerenable
		// Increment timercurrent
		io->IORegister[TIMERCURRENT]++; // Increment 32-bit current timer value

		// Check if timercurrent matches timermax
		if (io->IORegister[TIMERCURRENT] == io->IORegister[TIMERMAX]) { // timermax
			io->IORegister[IRQ0STATUS] = 1;   // Set irqstatus0 to trigger IRQ0
			io->IORegister[TIMERCURRENT] = 0; // Reset timercurrent to 0
		}
	}
}



