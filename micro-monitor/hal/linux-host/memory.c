/*
 * Emulate a 1K RAM to test Forth load/store commands
 */

#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"
#include "memory.h"
#include "forth.h"

// #define MEM_SIZE 1024

// uint8_t mem[MEM_SIZE];

shared_state_t *st = NULL;

// C!  ( val addr -- )
void do_c_store(void) 
{        
	int16_t addr = ustack_pop();

	int16_t val  = ustack_pop();

	if (addr >= 0 && addr < MEM_SIZE)
        	st->mem[addr] = (uint8_t) val;
    	else
		puts("ERR\n");
}

// C@  ( addr -- val )
void do_c_fetch(void) 
{        
	uint16_t addr = ustack_pop();

	if (addr >= 0 && addr < MEM_SIZE)
        	ustack_push(st->mem[addr]);
    	else 
		puts("ERR\n");
}

/* create shared mem segment and other virtual machine init */

void init_system(void)
{        
	int fd;

	fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);

	if (fd == -1) {

                fprintf(stderr, "Error opening shared memory '%s': %s\n", SHM_NAME, strerror(errno));
                exit(EXIT_FAILURE); 
        }

        ftruncate(fd, sizeof(shared_state_t));

        //shared_state_t *st = mmap(NULL, sizeof(*st), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

        st = mmap(NULL, sizeof(*st), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

	puts("1K RAM");
}
