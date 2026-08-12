/*
 * Steve Buer
 * Olympic College
 *
 * August 2026
 *
 * Forth-like commands
 *
 */

#include <stdio.h>
#include <stdint.h>
#include "forth.h"

#define MAXLINE 16
#define USER_STACK_SIZE 8

/* user stack */

uint16_t ustack[USER_STACK_SIZE];
int8_t usp = 0;

/* command execution */

void ustack_push(uint16_t n)
{
	ustack[usp] = n;
	usp++;
}

uint16_t ustack_pop(void)
{
	usp--;
	return ustack[usp + 1];
}

void ustack_clr(void)
{
	usp = 0;
}

void ustack_dup(void)
{
	ustack_push(ustack[usp - 1]);
}

void cmd_plus(void)
{
	uint16_t n1 = ustack_pop();
	uint16_t n2 = ustack_pop();
	ustack_push((uint16_t)(n1 + n2));
}

/* usage */

void forth_usage()
{
	puts(".c ( -- )\n");			/* clear stack */
	puts(".s ( -- )\n");			/* print stack */
	puts("c@ (addr -- )");			/* fetch byte */
	puts("c! (n addr -- )");		/* store byte */
	puts("+  (n n -- n)\n");		/* add */
	puts("-  (n n -- n)\n");		/* subtract */
	puts("*  (n n -- n)\n");		/* multiply */
}
