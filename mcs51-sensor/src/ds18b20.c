/*
 * MCS-51 Dallas DS18B20 temperature sensor reader
 *
 * Steve Buer, N7MKO
 * Olympic College
 *
 */

#include <mcs51/8052.h>
#include <stdint.h>
#include <stdio.h>
#include "system.h"
#include "onewire.h"

/* DS18B20 commands */

#define DS18B20_SKIP_ROM      0xCC
#define DS18B20_CONVERT_T     0x44
#define DS18B20_READ_SCRATCH  0xBE

uint16_t ds18b20_read_temp(void)
{
	uint8_t lsb, msb;
        uint16_t i;

	if (!ow_reset())
        	return 0xFFFF;

	ow_write_byte(DS18B20_SKIP_ROM);
	ow_write_byte(DS18B20_CONVERT_T);

	/* 12-bit conversion takes up to 750ms : fixme schedule this */

       	for (i = 0; i < 750; i++)
		ow_delay_us(1000);

	if (!ow_reset())
		return 0xFFFF;

	ow_write_byte(DS18B20_SKIP_ROM);
	ow_write_byte(DS18B20_READ_SCRATCH);

	lsb = ow_read_byte();
	msb = ow_read_byte();

	return ((uint16_t) msb << 8) | lsb;
}

/* scheduler task */

void task_temperature(void)
{
	int16_t raw;
	int16_t whole;
	uint16_t frac;
	char buf[16];

	raw = (int16_t) ds18b20_read_temp();

	if (raw == (int16_t) 0xFFFF) {

		puts("temp=err");
		return;
	}

	/* DS18B20 gives 1 LSB = 0.0625 deg C (12-bit default resolution) */

	whole = raw / 16;

	frac  = (uint16_t) (raw % 16);

	if (raw < 0 && frac != 0) {
        
		/* C truncates toward zero, fix sign for negative temps */
        
		whole -= 1;
		frac = 16 - frac;
    	}
	
	/* convert sixteenths to hundredths of a degree */

	frac = (frac * 625) / 100;   

	sprintf(buf, "temp=%d.%02u", whole, frac);
	
	puts(buf);
}
