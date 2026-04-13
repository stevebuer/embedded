/*
 * MCS-51 system routines
 *
 * Steve Buer, N7MKO
 * Olympic College
 *
 */

#include <mcs51/8052.h>
#include <stdint.h>
#include "system.h"

volatile uint16_t systick_ms = 0;

void timer0_ISR(void) __interrupt(1) 
{
	/* reload */

	TH0 = 0xFC;
	TL0 = 0x67;
	
	/* clear flag */

	TF0 = 0;

	/* increment 1ms */

	systick_ms++;
}

/* safe read 16 bit value, preserve caller interrupt state. */

uint16_t systick_read(void)
{
    __bit saved = EA;

    EA = 0;

    uint16_t tick = systick_ms;

    EA = saved;

    return tick;
}


/* Timer 0: 16 bit systick */

// 1 ms / 1.085 µs ≈ 921 counts
// 65536 − 921 = 64615 = 0xFC67

void init_systick()
{
	/* 1ms tick @ 11.0592 MHz */

	TH0 = 0xFC;        
	TL0 = 0x67;

	/* set timer0 mode */

	TMOD &= 0xF0;
	TMOD |= 0x01;

	/* enable timer0 interrupt */

	ET0 = 1;

	/* timer0 run */

	TR0 = 1;
}

uint16_t millis()
{
	return systick_read();
}
