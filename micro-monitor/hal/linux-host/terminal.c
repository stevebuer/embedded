/*
 * Linux vty i/o
 *
 */

#include <stdio.h>
#include <unistd.h>

#define MAXLINE 32

static char linebuf[MAXLINE];

/* our 'serial console' is a linux vty */

void init_serial()
{
	puts(ttyname(0));
}

char *readline_serial()
{
	fgets(linebuf, sizeof(linebuf), stdin);

	return linebuf;
}	
