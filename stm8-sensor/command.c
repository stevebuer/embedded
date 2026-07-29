/* 
 * Simple serial command processor for STM8
 *
 * Steve Buer, Olympic College 
 * July 2026
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "command.h"
#include "systick.h"

#define USER_STACK_SIZE 8

/* vars */

volatile char cmd_buf = 0;

static uint8_t arg_stack[USER_STACK_SIZE];

static uint8_t arg_sp = 0;

/* keep a little user stack for numeric command arguments: forth style */

static void stack_clear()
{
	memset(arg_stack, 0, sizeof(arg_stack));
	arg_sp = 0;
}

static void stack_push()
{
	if (arg_sp == USER_STACK_SIZE)
		return;

	/* atoi() and push */

	arg_stack[arg_sp++] = cmd_buf - 48;
}

static void stack_plus()
{
	uint8_t a, b = 0;

	a = arg_stack[--arg_sp];
	b = arg_stack[--arg_sp];

	arg_stack[arg_sp++] = a + b;
}

static void stack_mult()
{
	uint8_t a, b;

	a = arg_stack[--arg_sp];
	b = arg_stack[--arg_sp];

	arg_stack[arg_sp++] = a * b;
}

static void stack_dup()
{
	uint8_t a;

	a = arg_stack[arg_sp - 1];
	
	arg_stack[arg_sp++] = a;
}

static void stack_pop()
{
	char c;

	if (arg_sp == 0) {

		putchar('-'); putchar('1'); putchar('\n');
		return;
	}

	c = (char) arg_stack[--arg_sp];

	c += 48;

	putchar(c); putchar('\n');
}

/* format a char */

char *format_integer(uint8_t n)
{
	return n;
}

/* print systick */

void print_systick()
{
	printf("systick: %d", get_ticks());
}

/* called from main() */

void process_cmd()
{
	switch (cmd_buf) {

		case 'c':
			stack_clear();
			break;

		case 'd':
			stack_dup();
			break;

		case 't':
			print_systick();
			break;

		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		
			stack_push();
        		break;

		case '+':
			stack_plus();
			break;
		
		case '*':
			stack_mult();
			break;

		case '.':
			stack_pop();
			break;

		default:
			putchar('?'); putchar('\n');
        		break;
	}
}
