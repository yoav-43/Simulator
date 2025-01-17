#ifndef IO_H
#define IO_H

#include <stdint.h>
// Number of IO Registers
#define NUM_IO_REGISTERS 23
// IO Register Definitions
#define IRQ0ENABLE      0  // Enable IRQ0
#define IRQ1ENABLE      1  // Enable IRQ1
#define IRQ2ENABLE      2  // Enable IRQ2
#define IRQ0STATUS      3  // Status of IRQ0
#define IRQ1STATUS      4  // Status of IRQ1
#define IRQ2STATUS      5  // Status of IRQ2
#define IRQHANDLER      6  // Address of interrupt handler
#define IRQRETURN       7  // Address to return after interrupt
#define CLKS            8  // Clock cycle counter
#define LEDS            9  // LED state
#define DISPLAY7SEG     10 // 7-segment display
#define TIMERENABLE     11 // Enable/Disable timer
#define TIMERCURRENT    12 // Current timer value
#define TIMERMAX        13 // Timer max value
#define DISKCMD         14 // Disk command (Read/Write)
#define DISKSECTOR      15 // Disk sector
#define DISKBUFFER      16 // Disk buffer
#define DISKSTATUS      17 // Disk status
#define RESERVED1       18 // Reserved for future use
#define RESERVED2       19 // Reserved for future use
#define MONITORADDR     20 // Monitor address
#define MONITORDATA      21 // Monitor command
#define MONITORCMD      21 // Monitor command

// Define bit widths for each register
static const int IO_REGISTER_SIZES[NUM_IO_REGISTERS] = {1,  1,  1,  1,  1,  1,  12, 12, 32, 32, 32, 1, 32, 32, 2, 7, 12, 1, 32, 32, 16, 8, 1};

// Structure for I/O registers
typedef struct {
	uint32_t  IORegister[NUM_IO_REGISTERS];
	int halt_flag;
} IORegisters;


// Function declaration


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
