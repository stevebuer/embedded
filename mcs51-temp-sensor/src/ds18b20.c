/*
 * MCS-51 Dallas DS18B20 temperature sensor reader
 *
 * Steve Buer, N7MKO
 * Olympic College
 *
 */

#include <mcs51/8052.h>
#include <stdint.h>

uint16_t ds18b20_read_temp(void)
{
	return 0xffff;
}
