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
	
uint16_t last_read = 0;

main()
{
	init_systick();
	init_uart();
	init_timer();
	init_onewire();
	init_task();

	/* enable interrupts */

	EA = 1;

	/* program banner or other startup */

	printf("tick = %u\n", (unsigned) systick_read());

	/* program loop */

	while (1) {

		if (millis() - last_read >= 1000) {

			last_read = millis();

			uint16_t temp = ds18b20_read_temp();

        		// uart_print_temp(temp);
		}
    
		button_update();
	}
}
