/*
 * simple stm8 serial monitor
 */

#include "stm8s_conf.h"
#include "uart.h"
#include "cmd.h"
#include "i2c_bus.h"
#include "aht20.h"
#include "power.h"
#include "led.h"

/* i2c interface to aht20 driver */

aht20_ops_t aht20_ops = { i2c_write, i2c_read, i2c_delay, false, 0 };

#define EEPROM_AUTO_INTERVAL 0x4000UL
#define EEPROM_POWER_SAVE    0x4002UL

static void clock_init(void) 
{
	/* Use HSI at full speed (16 MHz) */

	CLK_DeInit();
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

static void gpio_init(void)
{
	led_init();
}

static void startup_led_blink(void)
{
	led_startup_blink();
}

int main(void) 
{
	uint16_t elapsed_seconds = 0;
	uint8_t auto_interval;
	uint8_t power_save;

	clock_init();

	gpio_init();

	uart_init(9600);

	startup_led_blink();

	i2c_bus_init();

	cmd_init();

	while (1) {

		cmd_poll();

		auto_interval = FLASH_ReadByte(EEPROM_AUTO_INTERVAL);
		power_save = FLASH_ReadByte(EEPROM_POWER_SAVE);
		if (auto_interval != 0 && !cmd_debug) {
			if (power_save != 0) {
				power_sleep_30s();
				cmd_poll();
				elapsed_seconds += 30U;
			} else {
				i2c_delay(1000UL);
				elapsed_seconds++;
			}

			if (elapsed_seconds >= ((uint16_t)auto_interval * 60U)) {
				cmd_auto_report();
				elapsed_seconds = 0;
			}
		} else {
			 elapsed_seconds = 0;
			i2c_delay(1000UL);
		}
    	}
}
