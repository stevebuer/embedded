#include "buttons.h"

/* GPIO Port A registers */
#define GPIOA_BASE         0x48000000
#define GPIOA_MODER        (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_PUPDR        (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))
#define GPIOA_IDR          (*(volatile uint32_t *)(GPIOA_BASE + 0x10))

/* RCC */
#define RCC_BASE           0x40021000
#define RCC_AHBENR         (*(volatile uint32_t *)(RCC_BASE + 0x14))

/* Button pins */
#define BUTTON_A_PIN       4
#define BUTTON_B_PIN       5

/* Debounce constants */
#define DEBOUNCE_COUNT     3  /* Need 3 stable reads to confirm press */

/* Button state tracking */
typedef struct {
    uint8_t current_state;      /* Current debounced state (1=pressed) */
    uint8_t previous_state;     /* Previous state for edge detection */
    uint8_t debounce_counter;   /* Debounce counter */
    uint32_t press_count;       /* Total number of presses */
} button_state_t;

static button_state_t button_states[BUTTON_COUNT] = {0};

/**
 * Initialize button GPIO inputs with pull-ups
 */
void button_init(void)
{
    /* Enable GPIOA clock */
    RCC_AHBENR |= (1 << 0);
    
    /* Configure PA4 and PA5 as inputs (mode 00) */
    GPIOA_MODER &= ~(3 << 8);   /* PA4 input */
    GPIOA_MODER &= ~(3 << 10);  /* PA5 input */
    
    /* Enable pull-ups (buttons are active low) */
    GPIOA_PUPDR |= (1 << 8);    /* PA4 pull-up */
    GPIOA_PUPDR |= (1 << 10);   /* PA5 pull-up */
}

/**
 * Read raw button pin state
 */
static uint8_t read_button_pin(button_t button)
{
    uint32_t pin = (button == BUTTON_A) ? BUTTON_A_PIN : BUTTON_B_PIN;
    uint32_t pin_bit = (GPIOA_IDR >> pin) & 1;
    
    /* Active low: pin=0 means pressed, so invert */
    return (pin_bit == 0) ? 1 : 0;
}

/**
 * Poll button state with debouncing
 */
uint8_t button_is_pressed(button_t button)
{
    if (button >= BUTTON_COUNT)
        return 0;
    
    button_state_t *btn = &button_states[button];
    uint8_t raw_state = read_button_pin(button);
    
    /* Debounce: count consecutive reads of same state */
    if (raw_state == btn->current_state)
    {
        /* State is stable */
        btn->debounce_counter = 0;
    }
    else
    {
        /* State is changing, increment counter */
        btn->debounce_counter++;
        
        if (btn->debounce_counter >= DEBOUNCE_COUNT)
        {
            /* State is confirmed different, update */
            btn->current_state = raw_state;
            btn->debounce_counter = 0;
            
            /* Count presses */
            if (btn->current_state == 1)  /* Just pressed */
                btn->press_count++;
        }
    }
    
    return btn->current_state;
}

/**
 * Edge detection: return 1 only on press transition
 */
uint8_t button_pressed_event(button_t button)
{
    if (button >= BUTTON_COUNT)
        return 0;
    
    button_state_t *btn = &button_states[button];
    
    /* Get current state (this also debounces) */
    uint8_t current = button_is_pressed(button);
    
    /* Detect transition from not-pressed to pressed */
    uint8_t event = (current == 1 && btn->previous_state == 0) ? 1 : 0;
    
    btn->previous_state = current;
    
    return event;
}

/**
 * Get button press count
 */
uint32_t button_get_press_count(button_t button)
{
    if (button >= BUTTON_COUNT)
        return 0;
    
    return button_states[button].press_count;
}
