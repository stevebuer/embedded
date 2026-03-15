#include <stdint.h>

#define RCC_AHBENR   (*(volatile uint32_t*)0x40021014)
#define GPIOA_MODER (*(volatile uint32_t*)0x48000000)
#define GPIOA_ODR    (*(volatile uint32_t*)0x48000014)

#define LED_PIN 4

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
    // Enable GPIOA clock
    RCC_AHBENR |= (1 << 17);
    // Set PA4 as output
    GPIOA_MODER &= ~(3 << (LED_PIN * 2));
    GPIOA_MODER |=  (1 << (LED_PIN * 2));

    while (1) {
        GPIOA_ODR ^= (1 << LED_PIN);
        delay_1s();
    }
    return 0;
}
