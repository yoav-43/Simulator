#define _CRT_SECURE_NO_WARNINGS
#include "disk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initialize the disk structure
void init_disk(Disk *disk) {
	memset(disk->data, 0, sizeof(disk->data));
	disk->timer = 0;
}

// Load disk content from an input file
void load_disk(const char *filename, Disk *disk) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		printf("Error: Could not open disk input file: %s\n", filename);
		exit(1);
	}

	char line[9]; // 8 hex digits + null terminator
	int sector = 0, byte_index = 0;
	while (fgets(line, sizeof(line), file)) {
		line[strcspn(line, "\r\n")] = '\0';
		uint8_t byte = (uint8_t)strtol(line, NULL, 16);
		disk->data[sector][byte_index] = byte;
		byte_index++;
		if (byte_index == SECTOR_SIZE) {
			byte_index = 0;
			sector++;
			if (sector == DISK_SECTORS) break;
		}
	}

	fclose(file);
	printf("Disk loaded from %s\n", filename);
}

// Write disk content to an output file
void write_disk(const char *filename, const Disk *disk) {
	FILE *file = fopen(filename, "w");
	if (!file) {
		printf("Error: Could not open disk output file: %s\n", filename);
		return;
	}

	for (int sector = 0; sector < DISK_SECTORS; sector++) {
		for (int byte = 0; byte < SECTOR_SIZE; byte++) {
			fprintf(file, "%02X\n", disk->data[sector][byte]);
		}
	}

	fclose(file);
	printf("Disk written to %s\n", filename);
}

// Read a sector from the disk into memory
void read_sector(Memory *memory, const IORegisters *io, const Disk *disk) {
	int sector = io->IORegister[15];
	int buffer = io->IORegister[16];

	if (sector < 0 || sector >= DISK_SECTORS) {
		printf("Error: Invalid sector number %d\n", sector);
		return;
	}

	for (int i = 0; i < SECTOR_SIZE; i++) {
		uint32_t word = read_data(memory, buffer + (i / 4)); 	// Read the 32-bit word from memory
		word &= ~(0xFF << ((3 - (i % 4)) * 8));					// Clear the target byte in the word by creating a mask
		word |= (disk->data[sector][i] << ((3 - (i % 4)) * 8));	// Insert the new byte from the disk
		write_data(memory, buffer + (i / 4), word);				// Write the updated word back to memory
	}
}

// Write a sector from memory to the disk
void write_sector(const Memory *memory, const IORegisters *io, Disk *disk) {
	int sector = io->IORegister[15];
	int buffer = io->IORegister[16];

	if (sector < 0 || sector >= DISK_SECTORS) {
		printf("Error: Invalid sector number %d\n", sector);
		return;
	}

	for (int i = 0; i < SECTOR_SIZE; i++) {
		uint32_t word = read_data(memory, buffer + (i / 4)); 			// Read the 32-bit word from memory
		disk->data[sector][i] = (word >> ((3 - (i % 4)) * 8)) & 0xFF;	// Extract the relevant byte from the word 
	}
}

// Handle disk commands and update DMA/IRQ
void handle_disk_command(Memory *memory, IORegisters *io, Disk *disk) {
	// Check if the disk is busy
	if (io->IORegister[17] == 1) {
		// If the disk is busy, decrement the timer
		if (disk->timer > 0) {
			disk->timer--;

			// When the timer reaches 0, complete the operation
			if (disk->timer == 0) {
				// Reset diskcmd and diskstatus
				io->IORegister[14] = 0; // Clear diskcmd
				io->IORegister[17] = 0; // Set diskstatus to "ready"

				// Raise the interrupt to signal that the operation is complete
				io->IORegister[4] = 1; // Set irq1status
			}
		}
		return; // Exit since the disk is still busy
	}

	// If the disk is ready, check if a new command is issued
	if (io->IORegister[14] != 0) {
		// Perform the operation specified in diskcmd
		switch (io->IORegister[14]) {
		case 1: // Read sector
			read_sector(memory, io, disk); // Perform the read operation
			break;

		case 2: // Write sector
			write_sector(memory, io, disk); // Perform the write operation
			break;

		default:
			break; // Ignore invalid commands
		}

		// Start the 1024-cycle countdown
		disk->timer = 1024;

		// Set diskstatus to "not ready"
		io->IORegister[17] = 1; // Disk is busy
	}
}


