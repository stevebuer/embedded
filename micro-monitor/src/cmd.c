/*
 * Steve Buer
 * Olympic College
 *
 * command interpreter
 *
 */

#include <stdio.h>
#include <stdint.h>

#define MAXLINE 16
#define USER_STACK_SIZE 8

char linebuf[MAXLINE];

const char *prompt = "cmd> ";

/* user stack for arg passing: KISS */

int16_t ustack[USER_STACK_SIZE];
int8_t usp = 0;

void ustack_push(int16_t n)
{
	ustack[usp] = n;
	usp++;
}

int16_t ustack_pop()
{
	usp--;
	return ustack[usp + 1];
}

void ustack_clr()
{
	usp = 0;
}
	

/* mix of stack and argument commands? */

void usage()
{
	printf("micro-mon: v0.1");
	printf("j <addr>");		/* jump to address */
	printf("p <addr>");		/* fetch */
	printf("o <addr> (n -- )");	/* store from user stack */

	printf("c");			/* clear user stack */
	printf("s");			/* print stack */
	printf("+ (n n -- n)");		/* add two numbers on user stack */
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

// Forth-like commands

void cmd_plus()
{
	int n1 = ustack_pop();
	int n2 = ustack_pop();

	return ustack_push(n1 + n2);
}
