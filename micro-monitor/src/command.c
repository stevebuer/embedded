/*
 * Steve Buer
 * Olympic College
 *
 * command interpreter
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "forth.h"

// const char *prompt = "> ";

/* mix of stack and argument commands? */

static void usage()
{
	printf("micro-mon: v0.1");
	printf("j <addr>");		/* jump to address */
	printf("p <addr>");		/* fetch */
	printf("o <addr> (n -- )");	/* store from user stack */

	printf("c");			/* clear user stack */
	printf("s");			/* print stack */
	printf("+ (n n -- n)");		/* add two numbers on user stack */
}

void init_command()
{
	puts("micro-mon: v0.1");
}

void process_command(char *s)
{
	if (*s == 'T' && *++s == 'E')
		puts("ok");
	else
		puts("err");
}

/*

char *next_token()
{
	// return next token in linebuf
}

int exec_command()
{
	// linked list of commands or switch calls 
}

void repl()
{
	// read line, parse command and arguments, execute 
}
*/

