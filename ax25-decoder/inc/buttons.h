#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>

/**
 * Button Input Support
 * Button A: PA4 (active low)
 * Button B: PA5 (active low)
 */

typedef enum {
    BUTTON_A = 0,
    BUTTON_B = 1,
    BUTTON_COUNT = 2
} button_t;

void button_init(void);

/**
 * Poll button state
 * Must be called regularly (at least every 20ms)
 * 
 * @param button Button to check (BUTTON_A or BUTTON_B)
 * @return 1 if pressed, 0 if released
 */

uint8_t button_is_pressed(button_t button);

/**
 * Check for button press event (edge detection)
 * Returns 1 only on transition from released to pressed
 * 
 * @param button Button to check
 * @return 1 if newly pressed, 0 otherwise
 */
uint8_t button_pressed_event(button_t button);

/**
 * Get button statistics
 * 
 * @param button Button to query
 * @return Number of times button has been pressed
 */
uint32_t button_get_press_count(button_t button);

#endif /* BUTTONS_H */
