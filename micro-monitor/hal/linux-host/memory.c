/*
 * Emulate a 1K RAM to test Forth load/store commands
 */

#include <stdio.h>
#include <stdint.h>
#include "dictionary.h"
#include "memory.h"
#include "forth.h"

#define MEM_SIZE 1024

uint8_t mem[MEM_SIZE];

// C!  ( val addr -- )
void do_c_store(void) 
{        
	int16_t addr = ustack_pop();

	int16_t val  = ustack_pop();

	if (addr >= 0 && addr < MEM_SIZE)
        	mem[addr] = (uint8_t) val;
    	else
		puts("ERR\n");
}

// C@  ( addr -- val )
void do_c_fetch(void) 
{        
	uint16_t addr = ustack_pop();

	if (addr >= 0 && addr < MEM_SIZE)
        	ustack_push(mem[addr]);
    	else 
		puts("ERR\n");
}
