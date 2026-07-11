/*
 * Blinky example with LL API
 */

#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"

void my_app(void) {
    // Enable GPIOC and GPIOA clocks
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

    // Configure PC8 and PC9 as output push-pull
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configure PA0 as input with pull-up
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    while (1) {
        if (LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0) == 0) { // Button pressed (active low)
            LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_9);
            LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_8);
        } else {
            LL_GPIO_TogglePin(GPIOC, LL_GPIO_PIN_8);
            LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_9);
        }
        // Delay for approximately 1 second (adjust based on system clock)
        // Assuming 8MHz system clock, this is roughly 1 second
        for (volatile int i = 0; i < 800000; i++);
    }
}
