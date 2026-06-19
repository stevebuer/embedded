/*
 * Program: Study Forth
 * Author: Steve Buer
 * Date: 02/2026
 */

#include <stdio.h>
#include <stdlib.h>

#define DATA_STACK_SIZE 32
#define MAX_LINE 64

/* data types */

typedef short int stack_t;

struct dict_word {

	struct dict_word *prev;
	const char *word;
	int flags;
	void * code;
	char data[64]; // where we compile strings, and XTs
};

struct dict_word *dict_head;

/* global vars */

stack_t data_stack[USER_STACK_SIZE];
char data_tos;

char input_line[MAX_LINE];

/* dictionary routines */

void add_word(const char *name, void (*code)(void), int flags) 
{ 
	dict_entry *e = malloc(sizeof(dict_entry)); 

	e->pre = dict_head; 
	e->name = name; 
	e->code = code; 
	e->flags = flags;
	
	dict_head = e; 
} 

/* data stack */

void reset_stacks()
{
	data_tos = 0;
}

void push_data(stack_t n)
{
	data_stack[data_tos] = n;
	data_tos++;
}	

stack_t pop_data()
{
	data_tos--;
	return data_stack[data_tos];
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

/* tokenizer: implements WORD primitive */

char *input;      // pointer to current position in input buffer
char token[64];   // temporary token buffer

char *next_token() 
{
	while (*input == ' ' || *input == '\t' || *input == '\n')
         	input++;

	if (*input == '\0')
		return NULL;

	char *t = token;

	while (input && input > ' ')
		t++ = input++;
    
	*t = '\0';

	return token;
}

/* parse number */



/* main interpreter loop */

int main()
{
    	char *tok;

	error:

	reset_stacks();

	while (1) {

		putchar('>'); putchar(' ');

		fgets(input_line, MAX_LINE, stdin);

		while ((tok = next_token()) != NULL) {

			if (*tok == 'q')
				return 0; /* easy quit for now */

			dict_word *w = find_word(tok);
			
			if (w) {
				
				execute(w);
        
			} else if (is_number(tok)) {
            
				push(parse_number(tok));
        
			} else {
            			
				printf("error");
				goto error;
        		}

    		} // while

	} // while

		/*

		if (input_line[0] == 'd')
			data_tos--;
		else if (input_line[0] == '+')
			prim_plus();
		else if (input_line[0] == '-')
			prim_minus();
		else if (input_line[0] == 's') {
			for (int i = 0; i < data_tos; i++)
				printf("%d ", data_stack[i]);
			putchar('\n');
		} else
			push_data(atoi(input_line)); 


		*/ 

		/* interpret token as a number */

		// parse next token

		/* search dictionary for token */

		// search_dict();


	}		

	return 0;
}
