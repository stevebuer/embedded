#include <stdint.h>

#define RCC_APB2ENR   (*(volatile uint32_t*)0x40021018)
#define GPIOC_CRH     (*(volatile uint32_t*)0x40011004)
#define GPIOC_ODR     (*(volatile uint32_t*)0x4001100C)
#define GPIOA_CRL     (*(volatile uint32_t*)0x40010800)
#define GPIOA_IDR     (*(volatile uint32_t*)0x40010808)
#define GPIOA_ODR     (*(volatile uint32_t*)0x4001080C)

const uint8_t led_pins[] = {8, 9};
int current_led = 0;
uint8_t button_pressed = 0;

// For 0.5 Hz blink (1s ON, 1s OFF) at 8 MHz, use a larger delay
void delay_1s(void)
{
    // Approximate 1 second delay for 8 MHz clock
    volatile uint32_t t = 800000;
    while (t--) {
        __asm__("nop");
    }
}

int main(void)
{
    // Enable GPIOC and GPIOA clocks
    RCC_APB2ENR |= (1 << 4) | (1 << 2);
    
    // Configure LEDs PC8 and PC9 as outputs (in CRH)
    GPIOC_CRH &= ~(0xFF << 0);  // Clear CNF and MODE for pins 8 and 9
    GPIOC_CRH |=  (0x33 << 0);  // Set MODE=11, CNF=00 for push-pull output
    
    // Configure PA0 as input with pull-down
    GPIOA_CRL &= ~(0xF << 0);
    GPIOA_CRL |=  (0x8 << 0);  // CNF=10 (pull), MODE=00 (input)
    GPIOA_ODR &= ~(1 << 0);    // pull-down
    
    // Turn on initial LED
    GPIOC_ODR |= (1 << led_pins[current_led]);

    while (1) {
        // Poll button (active high)
        uint8_t button_state = (GPIOA_IDR & 1);
        
        if (button_state && !button_pressed) {
            // Button just pressed, cycle to next LED
            GPIOC_ODR &= ~(1 << led_pins[current_led]);
            current_led = (current_led + 1) % 2;
            GPIOC_ODR |= (1 << led_pins[current_led]);
        }
        button_pressed = button_state;
        
        // Blink the current LED
        GPIOC_ODR ^= (1 << led_pins[current_led]);
        delay_1s();
    }
    return 0;
}