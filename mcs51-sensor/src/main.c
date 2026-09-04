/*
 * MCS-51 program to read DS1820B 1-wire temperature sensor
 *
 * Steve Buer, N7MKO
 * Olympic College
 *
 */

#include <mcs51/8052.h>
#include <stdint.h>
#include <stdio.h>
#include "system.h"
#include "onewire.h"
#include "uart.h"

/* globals */
	
uint16_t last_read = 0;

/* main */

main()
{
	init_uart();
	init_onewire();

	/* enable interrupts */

	// EA = 1;

	/* program banner or other startup */

	puts("mcs51-temp-sensor: firmware v0.0.1");
	
	/* program loop */

	while (1);
}
