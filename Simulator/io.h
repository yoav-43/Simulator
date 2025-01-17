#ifndef IO_H
#define IO_H

#include <stdint.h>

#define NUM_IO_REGISTERS 23

// Define bit widths for each register
static const int IO_REGISTER_SIZES[NUM_IO_REGISTERS] = {1,  1,  1,  1,  1,  1,  12, 12, 32, 32, 32, 1, 32, 32, 2, 7, 12, 1, 32, 32, 16, 8, 1};

// Structure for I/O registers
typedef struct {
	uint32_t  IORegister[NUM_IO_REGISTERS];
} IORegisters;


/*
-Functionality: Initialize all I/O registers to 0.
-parameter1: io - Pointer to the I/O registers structure to initialize.
*/
void init_io(IORegisters *io);


/*
-Functionality:  Read a value from an I/O register.
-parameter1: io - Pointer to the I/O registers structure.
-parameter2: reg_index - The index of the I/O register to read (0 to 22).
*/
uint32_t io_read(const IORegisters *io, int reg_index);


/*
-Functionality: Write a value to an I/O register, respecting its bit width.
-parameter1: io - Pointer to the I/O Registers structure.
-parameter2: reg_index - The index of the I/O register to wrute (0 to 22).
-parameter3: value - The value to set in the register.
*/
void io_write(IORegisters *io, int reg_index, uint32_t value);


/*
-Functionality: Increment the clock counter.
-parameter1: io - Pointer to the I/O registers structure.
*/
void increment_clock(IORegisters *io);


/*
-Functionality:  * Update the timer registers.
-parameter1: io - Pointer to the I/O registers structure.
*/
void update_timer(IORegisters *io);

#endif 
