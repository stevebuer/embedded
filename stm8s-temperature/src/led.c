#include "stm8s_conf.h"
#include "i2c_bus.h"
#include "led.h"

#define LED_PORT GPIOD
#define LED_PIN GPIO_PIN_3
#define LED_ON_TIME_MS 100UL
#define LED_STARTUP_PAUSE_MS 100UL

static void led_on(void)
{
	GPIO_WriteHigh(LED_PORT, LED_PIN);
}

static void led_off(void)
{
	GPIO_WriteLow(LED_PORT, LED_PIN);
}

static void led_blink(void)
{
	led_on();
	i2c_delay(LED_ON_TIME_MS);
	led_off();
}

void led_init(void)
{
	GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
	led_off();
}

void led_startup_blink(void)
{
	uint8_t count;

	for (count = 0; count < 3; count++) {
		led_blink();
		i2c_delay(LED_STARTUP_PAUSE_MS);
	}
}

void led_sensor_read_blink(void)
{
	led_blink();
}
