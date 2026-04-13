/*
 * MCS-51 delay routines
 *
 * Steve Buer, N7MKO
 * Olympic College
 *
 */

#include <mcs51/8052.h>
#include <stdio.h>
#include <stdint.h>
#include "system.h"

void delay_ms(uint16_t ms) 
{
	uint16_t start = systick_read();

	while ((systick_read() - start) < ms);
}
