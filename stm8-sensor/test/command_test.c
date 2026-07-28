/* 
 * Test simple command processor on host
 *
 * Steve Buer, Olympic College 
 * July 2026
 *
 * compile: gcc ../command.c command_test.c
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

void process_cmd(void);

char cmd_buf;

int main(int argc, char **argv)
{
	int c;

	while (1) {
		
		c = getchar();

		printf("debug: char (%c) : dec (%d)\n", c, c);

		getchar(); // skip newline

		cmd_buf = (char) c;

		process_cmd();
	}

	return 0;
}
