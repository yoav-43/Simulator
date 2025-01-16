#ifndef EXECUTION_H
#define EXECUTION_H

#include <stdint.h>
#include "registers.h" // To access and modify the registers
#include "memory.h"    // For memory-related instructions
#include "io.h" // To access and modify the io registers
#include "instruction_fetch.h" // For the pc handaling
#include "instruction_decode.h" // For the decoded instruction


// Function declaration

/*
-Functionality: Executes a decoded instruction.
-parameter1: decoded_instruction - Pointer to the decoded instruction.
-parameter2: registers - Pointer to the Registers structure.
-parameter3: memory - Pointer to the Memory structure.
-parameter4: pc - Pointer to the Program counter.

*/
void execute_instruction(const Instruction *decoded_instruction, Registers *registers, Memory *memory, uint16_t *pc);


#endif 

