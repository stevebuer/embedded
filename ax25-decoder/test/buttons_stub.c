/*
 * Button Input Stub for Host Testing
 * 
 * This is a stub implementation of the button driver for host-based testing.
 * Button input is not used during WAV file testing.
 */

#include <stdint.h>
#include "../inc/buttons.h"

/**
 * Initialize button inputs (stub)
 */
void button_init(void)
{
	/* Nothing to do for host */
}

/**
 * Poll button state (stub - always returns not pressed)
 */
uint8_t button_is_pressed(button_t button)
{
	(void)button;  /* Unused */
	return 0;      /* Button not pressed */
}
