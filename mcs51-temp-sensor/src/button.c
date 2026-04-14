/*
 * MCS-51 button routines
 *
 * Steve Buer, N7MKO
 * Olympic College
 *
 */

#include <mcs51/8052.h>
#include "system.h"

#define BUTTON_1 P2_0

__bit button_flag = 0;

void task_button(void)
{
	/* button press is active low: (BUTTON_1 == 0) */

	if (BUTTON_1 == 0) {
	
		/* debounce delay */

		delay_ms(20);

		/* check if button is still pressed */

		if (BUTTON_1 == 0) {

			/* wait for button release */

            		while (BUTTON_1 == 0);

			/* set button event flag */

			button_flag = 1;
        	}
    	}
}
