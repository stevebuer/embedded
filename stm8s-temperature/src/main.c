/*
 * simple stm8 serial monitor
 */

#include "stm8s_conf.h"
#include "uart.h"
#include "cmd.h"
#include "i2c_bus.h"
#include "aht20.h"

/* i2c interface to aht20 driver */

aht20_ops_t aht20_ops = { i2c_write, i2c_read, i2c_delay, false, 0 };

#define EEPROM_AUTO_INTERVAL 0x4000UL

static void clock_init(void) 
{
	/* Use HSI at full speed (16 MHz) */

	CLK_DeInit();
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

static void gpio_init(void)
{
	/*
	 * Board LED is on PB5, which is also I2C SDA on STM8S003.
	 * Avoid configuring PB5 as push-pull when using I2C.
	 */
}

static void startup_led_blink(void)
{
	/* PB5 LED conflicts with SDA; no blink while using I2C on PB5. */
}

int main(void) 
{
	uint8_t elapsed_minutes = 0;
	uint8_t auto_interval;

	clock_init();

	gpio_init();

	uart_init(9600);

	startup_led_blink();

	i2c_bus_init();

	cmd_init();

	while (1) {

		cmd_poll();

		auto_interval = FLASH_ReadByte(EEPROM_AUTO_INTERVAL);
		if (auto_interval != 0) {
			i2c_delay(60000UL);
			if (++elapsed_minutes >= auto_interval) {
				cmd_auto_report();
				elapsed_minutes = 0;
			}
		} else {
			elapsed_minutes = 0;
			i2c_delay(1000UL);
		}
    	}
}
