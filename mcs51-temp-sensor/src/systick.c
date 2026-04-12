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
    TH0 = 0xFC;        // Reload high byte
    TL0 = 0x67;        // Reload low byte

    systick_ms++;      // Increment millisecond counter
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

void init_sys_tick()
{
	/* 1ms tick @ 11.0592 MHz */

	TH0 = 0xFC;        
	TL0 = 0x67;

	TMOD &= 0xF0;      // Clear Timer0 bits
	TMOD |= 0x01;      // Timer0 Mode 1 (16-bit)

	ET0 = 1;           // Enable Timer0 interrupt
	TR0 = 1;           // Start Timer0
}

void init_systick()
{
	init_sys_tick();
}

uint16_t millis()
{
	// todo
	return 0xffff;
}
