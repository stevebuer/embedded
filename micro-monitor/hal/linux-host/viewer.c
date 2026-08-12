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
#include <ctype.h>

#include "dictionary.h"
#include "memory.h"

/* memory display */

#define MEM_SIZE 1024
#define MEM_COLS 16
#define MEM_ROWS (MEM_SIZE / MEM_COLS)     /* 1024 / 16 = 64 rows total */
#define MEM_PANE_X 45
#define MEM_PANE_ROWS 16                   /* rows visible per page */
#define MEM_PAGES ((MEM_ROWS + MEM_PANE_ROWS - 1) / MEM_PANE_ROWS)  /* = 4 */

int mem_page = 0;   /* 0..3 */

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

int main(int argc, char *argv[])
{
	int fd;

	fd = shm_open(SHM_NAME, O_RDONLY, 0666);

	if (fd == -1) {

		fprintf(stderr, "Error opening shared memory '%s': %s\n", SHM_NAME, strerror(errno));
		exit(EXIT_FAILURE); 
	}

	shared_state_t *st = mmap(NULL, sizeof(*st), PROT_READ, MAP_SHARED, fd, 0);

	initscr();

	nodelay(stdscr, TRUE);   /* call once after initscr() */

	while (1) {

		int ch = getch();

		if (ch == 'm')
			mem_page = (mem_page + 1) % MEM_PAGES;
		else if (ch == 'q')
			break;

		clear();
		
		mvprintw(0, 0, "IP: %d   DSP: %d   gen: %u", st->ip, st->dsp, st->generation);

		for (int i = 0; i < st->ntok; i++)
        		mvprintw(2+i, 0, "%s[%d] %s", (i == st->ip ? "->" : "  "), i, token_name(st->prog[i]));

    		for (int i = 0; i < st->dsp; i++)
			mvprintw(2, 30 + i*6, "%d", st->dstack[i]);

		/* ---- memory pane ---- */

		int row_start = mem_page * MEM_PANE_ROWS;

		mvprintw(0, MEM_PANE_X, "MEM page %d/%d  ('m' next, 'q' quit)", mem_page + 1, MEM_PAGES);

		for (int row = 0; row < MEM_PANE_ROWS && (row_start + row) < MEM_ROWS; row++) {

			int addr = (row_start + row) * MEM_COLS;
			int y = 2 + row;
			int x = MEM_PANE_X;

			mvprintw(y, x, "%04d:", addr);   /* back to 4 digits, max address is 1023 */
			x += 6;

			for (int col = 0; col < MEM_COLS; col++) {

				uint8_t byte = st->mem[addr + col];
				mvprintw(y, x, "%02X", byte);
				x += 3;
			}

			x += 1;

			for (int col = 0; col < MEM_COLS; col++) {

				uint8_t byte = st->mem[addr + col];
				mvaddch(y, x, isprint(byte) ? byte : '.');
				x++;
			}
		}
		
		refresh();

		usleep(50000);   // ~20fps poll
	}

	endwin();
}
