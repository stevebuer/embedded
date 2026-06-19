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

static inline void led_on()
{
	LED_PIN = 0;
}

static inline void led_off()
{
	LED_PIN = 1;
}

static inline void led_toggle()
{
	LED_PIN = !LED_PIN;
}

void task_led(void)
{
	LED_PIN ^= 1;
}
