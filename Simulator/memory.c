#define _CRT_SECURE_NO_WARNINGS
#include "memory.h"
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>


// Initialize the memory
void init_memory(Memory *memory) {
	// Set all instruction memory lines to 0
	for (int i = 0; i < INSTRUCTION_MEM_DEPTH; i++) {
		memset(memory->instructions[i], 0, sizeof(memory->instructions[i]));
	}

	// Set all data memory lines to 0
	for (int i = 0; i < DATA_MEM_DEPTH; i++) {
		memory->data[i] = 0;
	}
}

// Load the instruction memory
void load_instruction_memory(const char *filename, Memory *memory) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		printf("Error: Could not open instruction memory file: %s\n", filename);
		exit(1);
	}

	char line[13]; // 12 hex digits + null terminator
	int address = 0;
	while (fgets(line, sizeof(line), file) && address < INSTRUCTION_MEM_DEPTH) {
		line[strcspn(line, "\r\n")] = '\0'; // Remove newline characters
		for (int i = 0; i < 6; i++) {
			char byte_str[3] = { line[i * 2], line[i * 2 + 1], '\0' };
			memory->instructions[address][i] = (uint8_t)strtol(byte_str, NULL, 16);
		}
		address++;
	}

	fclose(file);
	printf("Instruction memory loaded from %s\n", filename);
}

// Load the data memory
void load_data_memory(const char *filename, Memory *memory) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		printf("Error: Could not open data memory file: %s\n", filename);
		exit(1);
	}

	char line[9]; // 8 hex digits + null terminator
	int address = 0;
	while (fgets(line, sizeof(line), file) && address < DATA_MEM_DEPTH) {
		line[strcspn(line, "\r\n")] = '\0'; // Remove newline characters
		memory->data[address] = (uint32_t)strtol(line, NULL, 16);
		address++;
	}

	fclose(file);
	printf("Data memory loaded from %s\n", filename);
}

// Write the data memory
void write_data_memory(const char *filename, const Memory *memory) {
	FILE *file = fopen(filename, "w");
	if (!file) {
		printf("Error: Could not open data memory output file: %s\n", filename);
		exit(1);
	}

	for (int i = 0; i < DATA_MEM_DEPTH; i++) {
		fprintf(file, "%08X\n", memory->data[i]);
	}

	fclose(file);
	printf("Data memory written to %s\n", filename);
}


// Read a 48-bit instruction from instruction memory
const uint8_t *read_instruction(const Memory *memory, int address) {
	if (address < 0 || address >= INSTRUCTION_MEM_DEPTH) {
		printf("Error: Invalid instruction memory address %d\n", address);
		return NULL;
	}
	return memory->instructions[address];
}

// Read a 32-bit word from data memory
uint32_t read_data(const Memory *memory, int address) {
	if (address < 0 || address >= DATA_MEM_DEPTH) {
		printf("Error: Invalid data memory address %d\n", address);
		return 0;
	}
	return memory->data[address];
}

// Write a 32-bit word to data memory
void write_data(Memory *memory, int address, uint32_t value) {
	if (address < 0 || address >= DATA_MEM_DEPTH) {
		printf("Error: Invalid data memory address %d\n", address);
		return;
	}
	memory->data[address] = value;
}
