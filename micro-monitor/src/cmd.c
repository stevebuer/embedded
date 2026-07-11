/*
 * Steve Buer
 * Olympic College
 *
 * command interpreter
 *
 */

#include <stdio.h>

#define MAXLINE 16
#define USER_STACK_SIZE 16
#define USER_STACK_ADDR 0xF0

char linebuf[MAXLINE];

const char *prompt = "cmd> ";

/* user stack for arg passing: KISS */

unsigned char *ustack = USER_STACK_ADDR;
static int ustack_idx = 0;

void ustack_push(int n)
{
	ustack[ustack_idx] = n;
	ustack_idx++;
}

int ustack_pop()
{
	ustack_idx--;
	return ustack[ustack_idx + 1];
}

void ustack_clr()
{
	ustack_idx = 0;
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
