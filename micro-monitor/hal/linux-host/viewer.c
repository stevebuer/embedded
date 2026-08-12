/*
 * Debug Viewer
 */

#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ncurses.h>
#include <stddef.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dictionary.h"
#include "memory.h"

/* print token name string */

const char *token_name(token_t t) 
{
	static char buf[32];   // static so it survives the return; fine for single-threaded debug use

	switch (t.type) {

		case TOK_WORD:
			return words[t.u.word_idx].name;      // pull the name back out of the dictionary

		case TOK_LIT:
			snprintf(buf, sizeof(buf), "%d", t.u.lit);
			return buf;

		case TOK_AGAIN:
			snprintf(buf, sizeof(buf), "AGAIN -> %d", t.u.jump_target);
			return buf;

		case TOK_ZEXIT:
			return "?EXIT";

		case TOK_JUMP:
			return (t.u.jump_target < 0) ? "EXIT" : "JUMP";
		
		default:
			return "???";
    }
}

/* main */

int main(int argc, char **argv[])
{
	int fd;

	fd = shm_open(SHM_NAME, O_RDONLY, 0666);

	if (fd == -1) {

		fprintf(stderr, "Error opening shared memory '%s': %s\n", SHM_NAME, strerror(errno));
		exit(EXIT_FAILURE); 
	}

	shared_state_t *st = mmap(NULL, sizeof(*st), PROT_READ, MAP_SHARED, fd, 0);

	initscr();

	while (1) {

		clear();
		
		mvprintw(0, 0, "IP: %d   DSP: %d   gen: %u", st->ip, st->dsp, st->generation);

		for (int i = 0; i < st->ntok; i++)
        		mvprintw(2+i, 0, "%s[%d] %s", (i == st->ip ? "->" : "  "), i, token_name(st->prog[i]));

    		for (int i = 0; i < st->dsp; i++)
			mvprintw(2, 30 + i*6, "%d", st->dstack[i]);
		
		refresh();

		usleep(50000);   // ~20fps poll
	}
}
