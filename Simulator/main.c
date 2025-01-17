#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>   // For fixed-width integer types 
#include <stdio.h>    // For input/output operations 
#include <stdlib.h>   // For memory allocation and exit handling
#include "registers.h"   
#include "memory.h"      
#include "io.h"         
#include "disk.h" 
#include "monitor.h" 
#include "interrupts.h"  
#include "instruction_fetch.h"
#include "instruction_decode.h" 
#include "execution.h" 


void write_trace(FILE *file, uint16_t pc, uint32_t instruction, const Registers *registers) {
	// Print PC (3 hexadecimal digits) and instruction encoding (12 hexadecimal digits)
	fprintf(file, "%03X %08X%03X ", pc, instruction >> 12, instruction & 0xFFF);

	// Print all registers R0 to R15
	for (int i = 0; i <= 15; i++) {
		fprintf(file, "%08X ", registers->regs[i]);
	}

	// End the line
	fprintf(file, "\n");
}


 // The simulator fetch-decode-exe loop
void simulator_main_loop(Registers *registers, Memory *memory, IORegisters *io, Disk *disk, IRQ2Data *irq2, Monitor *monitor, const char *trace_filename, const char *hwregtrace_filename, const char *leds_filename, const char *display7seg_filename) {
	uint16_t pc = 0;        // Program counter (12-bit)
	int in_isr = 0;         // ISR state (0 = not in ISR, 1 = in ISR)
	Instruction decoded;    // Decoded instruction

	// Open trace and hardware register trace files
	FILE *trace_file = fopen(trace_filename, "w");
	if (!trace_file) {
		printf("Error: Could not open trace file: %s\n", trace_filename);
		return;
	}

	FILE *hwregtrace_file = fopen(hwregtrace_filename, "w");
	if (!hwregtrace_file) {
		printf("Error: Could not open hardware register trace file: %s\n", hwregtrace_filename);
		fclose(trace_file); // Close the already opened file
		return;
	}

	// Open LED and 7-segment display files
	FILE *leds_file = fopen(leds_filename, "w");
	if (!leds_file) {
		printf("Error: Could not open LEDs file: %s\n", leds_filename);
		fclose(trace_file);
		fclose(hwregtrace_file);
		return;
	}

	FILE *display7seg_file = fopen(display7seg_filename, "w");
	if (!display7seg_file) {
		printf("Error: Could not open 7-segment display file: %s\n", display7seg_filename);
		fclose(trace_file);
		fclose(hwregtrace_file);
		fclose(leds_file);
		return;
	}

	while (io->halt_flag) {
		// Increment the clock register
		increment_clock(io);

		// Update the timer
		update_timer(io);

		// Check and trigger IRQ2 based on the current clock cycle
		check_and_trigger_irq2(io, irq2, io->IORegister[CLKS]);
		
		// Manage disk operations (e.g., read/write tasks)
		handle_disk_command(memory, io, disk);

		// Handle interrupts if any are pending
		handle_interrupts(io, &pc, &in_isr);
		
		// Handle monitor commands
		if (io->IORegister[MONITORCMD] == 1) {
			write_pixel(monitor, io); // Write the pixel to the monitor
		}

		// Fetch the next instruction using the 12-bit PC
		const uint8_t* instruction = fetch_instruction(memory, &pc);

		// Write to the trace file before executing the instruction
		write_trace(trace_file, pc, *instruction, registers);

		// Decode the fetched instruction
		decode_instruction(instruction, &decoded, registers);

		// Execute the decoded instruction
		execute_instruction(&decoded, registers, memory, io, &pc, &in_isr);

		/*
		// Log hardware register changes
		log_hardware_registers(hwregtrace_file, io);

		// Update LEDs and 7-segment display logs if needed
		log_leds(leds_file, io);
		log_display7seg(display7seg_file, io);
		*/
	}

	// Close all opened files
	fclose(trace_file);
	fclose(hwregtrace_file);
	fclose(leds_file);
	fclose(display7seg_file);
}

// Writes the values of registers R3–R15 to an output file.
void write_registers(const char *filename, const Registers *registers) {
	FILE *file = fopen(filename, "w");
	if (!file) {
		printf("Error: Could not open %s for writing.\n", filename);
		return;
	}

	// Write the values of registers R3–R15 (indices 3 to 15)
	for (int i = 3; i <= 15; i++) {
		fprintf(file, "%08X\n", registers->regs[i]);
	}

	fclose(file);
}

int main(int argc, char *argv[]) {
	// Ensure the correct number of arguments are passed
	if (argc != 15) {
		printf("Usage: %s imemin.txt dmemin.txt diskin.txt irq2in.txt dmemout.txt regout.txt trace.txt hwregtrace.txt cycles.txt leds.txt display7seg.txt diskout.txt monitor.txt monitor.yuv\n", argv[0]);
		return 1;
	}

	// Parse input and output file names
	const char *imemin = argv[1];      // Instruction memory input file
	const char *dmemin = argv[2];      // Data memory input file
	const char *diskin = argv[3];      // Disk content input file
	const char *irq2in = argv[4];      // IRQ2 events input file
	const char *dmemout = argv[5];     // Data memory output file
	const char *regout = argv[6];      // Registers output file
	const char *trace = argv[7];       // Instruction trace output file
	const char *hwregtrace = argv[8];  // Hardware register trace output file
	const char *cycles = argv[9];      // Clock cycle count output file
	const char *leds = argv[10];       // LED state output file
	const char *display7seg = argv[11];// 7-segment display output file
	const char *diskout = argv[12];    // Disk content output file
	const char *monitor_txt = argv[13];// Monitor text output file
	const char *monitor_yuv = argv[14];// Monitor YUV binary output file

	// Initialize general-purpose registers
	Registers registers; 
	init_registers(&registers);

	// Initialize instruction and data memory
	Memory memory;       
	init_memory(&memory);
	load_instruction_memory(imemin, &memory);  
	load_data_memory(dmemin, &memory);         

	// Initialize IO registers
	IORegisters io;      
	init_io(&io);

	// Initialize disk structure
	Disk disk;           
	init_disk(&disk);
	load_disk(diskin, &disk);                  

	// Initialize monitor structure
	Monitor monitor;     
	init_monitor(&monitor);

	// Initialize IRQ2 events
	IRQ2Data irq2;       
	load_irq2_events(irq2in, &irq2);          

	// Call the main simulation loop
	simulator_main_loop(&registers, &memory, &io, &disk, &irq2, &monitor, trace, hwregtrace, leds, display7seg);

	// Write output files
	write_data_memory(dmemout, &memory);       // Write data memory output
	write_registers(regout, &registers);	   // Write registers output
	write_monitor_text(&monitor, monitor_txt); // Write monitor text file
	write_monitor_yuv(&monitor, monitor_yuv);  // Write monitor YUV file
	write_disk(diskout, &disk);                // Write disk content output
	write_cycles(cycles, &io);				   // Write clock cycles output

	printf("Simulation completed.\n");
	return 0;
}

