/*
 * MCS-51 serial command interpreter
 *
 * Steve Buer, N7MKO
 * Olympic College
 *
 */

#include <mcs51/8052.h>
#include <stdint.h>
#include <stdio.h>
#include "systick.h"
#include "cmd.h"

/* flag from uart isr */

volatile __bit cmd_flag = 0;

/* command character from uart isr */

char cmd_buf = 0;

/* task handler */

void task_cmd(void)
{
	if (!cmd_flag)
		return;

	cmd_flag = 0;

	switch(cmd_buf) {

		case 'b':
		case 'c':
		case 'd':
		case 'f':
		case 'h':
		case 'i':
		case 'l':
		case '?':
		case 'o':
		case 's':
			goto err;
		case 't':
			printf("tick=%u\n", (unsigned) systick_read());
			break;
	err:
			puts("err?");
			
	}
}
