/*
 * MCS-51 led routines
 *
 * Steve Buer, N7MKO
 * Olympic College
 *
 */

#include <mcs51/8052.h>
#include "system.h"

#define LED_PIN P2_2

void task_led(void)
{
	LED_PIN ^= 1;
}
