/*
 * Program: Study Forth
 * Author: Steve Buer
 * Date: 02/2026
 */

#include <stdio.h>
#include <stdlib.h>

#define USER_STACK_SIZE 32
#define MAX_LINE 64

/* data types */

typedef short int stack_t;

struct dict_entry {

	char word; // simple 1 char word for arithmetic
	int type; // no colon for now
	void * code; // fxn pointer to code
	char data[64]; // where we compile strings, and XTs
};

/* global vars */

stack_t user_stack[USER_STACK_SIZE];
char user_tos;

char input_line[MAX_LINE];

char dictionary[] = {'+', '-', '*', '/' };

/* user stack */

void reset_stacks()
{
	user_tos = 0;
}

void push_user(stack_t n)
{
	user_stack[user_tos] = n;
	user_tos++;
}	

stack_t pop_user()
{
	user_tos--;
	return user_stack[user_tos];
}

/* primitives */

void prim_plus()
{
	stack_t a, b;

	a = pop_user();
	b = pop_user();

	push_user(a + b);
}

void prim_minus()
{
	stack_t a, b;
	
	a = pop_user();
	b = pop_user();
		
	push_user(a - b);
}

/* main interpreter loop */

int main()
{
	reset_stacks();

	while (1) {

		putchar('>'); putchar(' ');

		fgets(input_line, MAX_LINE, stdin);

		if (input_line[0] == 'd')
			user_tos--;
		else if (input_line[0] == '+')
			prim_plus();
		else if (input_line[0] == '-')
			prim_minus();
		else if (input_line[0] == 'q')
			return 0;
		else if (input_line[0] == 's') {
			for (int i = 0; i < user_tos; i++)
				printf("%d ", user_stack[i]);
			putchar('\n');
		} else
			push_user(atoi(input_line)); /* interpret token as a number */

		// parse next token

		/* search dictionary for token */

		// search_dict();


	}		

	return 0;
}
