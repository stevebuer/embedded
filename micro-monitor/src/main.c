/*
 * Steve's micro-monitor
 */

#include <stdio.h>
#include "machine.h"

int main(void)
{
	char *line = NULL;

	init_system();

	init_serial();

	init_command();

	while (1) {

		putchar('>'); putchar(' ');

		line = readline_serial();

		process_command(line);
	}

	return 0;
}
