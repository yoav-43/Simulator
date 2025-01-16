#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include "memory.h"
#include "io.h"

// Disk constants
#define DISK_SECTORS 128  // Number of sectors in the disk
#define SECTOR_SIZE 512   // Bytes per sector

// Disk structure
typedef struct {
	uint8_t data[DISK_SECTORS][SECTOR_SIZE]; // Disk sectors
	int timer;                               // Timer for disk operations
} Disk;

// Function declarations

/*
-Functionality: Initializes the disk structure.
-parameter1: disk - Pointer to the Disk structure.
*/
void init_disk(Disk *disk);

/*
-Functionality: Load the disk content from an input file.
-parameter1: filename - Name of the input file (diskin.txt).
-parameter2: disk - Pointer to the Disk structure.
*/
void load_disk(const char *filename, Disk *disk);

/*
-Functionality: Write the disk content to an output file.
-parameter1: filename - Name of the output file (diskout.txt).
-parameter2: disk - Pointer to the Disk structure.
*/
void write_disk(const char *filename, const Disk *disk);

/*
-Functionality: Handle a read sector operation from the disk.
-parameter1: memory - Pointer to the Memory structure.
-parameter2: io - Pointer to the IORegisters structure.
-parameter3: disk - Pointer to the Disk structure.
*/
void read_sector(Memory *memory, const IORegisters *io, const Disk *disk);

/*
-Functionality: Handle a write sector operation to the disk.
-parameter1: memory - Pointer to the Memory structure.
-parameter2: io - Pointer to the IORegisters structure.
-parameter3: disk - Pointer to the Disk structure.
*/
void write_sector(const Memory *memory, const IORegisters *io, Disk *disk);

/*
-Functionality: Handle the disk command and update DMA or IRQs as needed.
-parameter1: memory - Pointer to the Memory structure.
-parameter2: io - Pointer to the IORegisters structure.
-parameter3: disk - Pointer to the Disk structure.
*/
void handle_disk_command(Memory *memory, IORegisters *io, Disk *disk);

#endif 
