/*
 * MCS-51 serial command interpreter
 *
 * Steve Buer, N7MKO
 * Olympic College
 * July 2026
 *
 */

#include <mcs51/8052.h>
#include <stdint.h>
#include <stdio.h>
#include "onewire.h"
#include "command.h"

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
			break;
		case 'o':
			printf("onewire scan: %d\n", ow_reset());
			break;
		case 't':
			break;
		default:
			puts("err?");
			
	}
}
