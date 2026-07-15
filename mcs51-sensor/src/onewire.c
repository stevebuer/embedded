/*
 * Dallas 1-wire routines
 *
 * Steve Buer, N7MKO
 * Olympic College
 *
 */

#include <mcs51/8052.h>
#include <stdio.h>
#include <stdint.h>

/* bit SFR for 1-wire bus */

#define DQ P1_0

/* 1-wire command bytes */

#define CMD_READ_ROM 0x33

void init_onewire(void)
{
	puts("Onewire pin init: P1_0");
    	DQ = 1;
}

/* 11.059 Mhz crystal gives ~1.085 us delay : keep our own for now */

void ow_delay_us(unsigned int us)
{
	while (us--) {
	
		__asm
		nop
		nop
		__endasm;
	}
}

/* reset pulse + presence detect: 1 if presence pulse, 0 of not */

unsigned char ow_reset(void)
{
	unsigned char presence;

	/* hold bus low for reset pulse (480-960us) */

	DQ = 0;
	ow_delay_us(480);
	DQ = 1;

	/* wait for device to pull low (presence pulse starts within 15-60us) */

	ow_delay_us(60);
	
	/* sample presence: low = device present, high = no device */

	presence = DQ;

	/* finish the 480us+ reset/presence slot */

	ow_delay_us(420);

	return (presence == 0) ? 1 : 0;
}

/* write a single bit */

void ow_write_bit(unsigned char bit)
{
	/* start slot: low */

	DQ = 0;              

	if (bit) {

		/* send 1: short low, long high */

		ow_delay_us(6);
		DQ = 1;
		ow_delay_us(64);

	} else {

		/* send 0: hold full slot low wth 4us recovery */

		ow_delay_us(60);
		DQ = 1;
		ow_delay_us(4);
	}
}

/* read a single bit */

unsigned char ow_read_bit(void)
{
	unsigned char bit;

	/* start slot */

	DQ = 0;
	ow_delay_us(2);
	DQ = 1;
	
	/* wait before sample point (~13-15us from slot start) */

	ow_delay_us(9);             

	/* read sample */

	bit = DQ;
	
	/* wait for slot end (~60us total) */

	ow_delay_us(50);            

	return bit;
}

/* write a byte, LSB first */

void ow_write_byte(unsigned char data)
{
	unsigned char i;

	for (i = 0; i < 8; i++) {

		ow_write_bit(data & 0x01);
		data >>= 1;
	}
}

/* read a byte, LSB first */

unsigned char ow_read_byte(void)
{
	unsigned char i, data = 0;

	for (i = 0; i < 8; i++) {

		data >>= 1;

		if (ow_read_bit())
			data |= 0x80;
	}

	return data;
}

/* read 64-bit ROM id */

void ow_read_rom(uint8_t *rom)
{
	uint8_t i;

	/* no device present */

	if (!ow_reset())
		return;  

	/* Read ROM */

	ow_write_byte(CMD_READ_ROM);  

	for (i = 0; i < 8; i++)
		rom[i] = ow_read_byte();
}

/* our bus only has a single device for now */

void ow_scanbus(void)
{
	uint8_t i, rom[8];

	ow_read_rom(rom);

	for (i = 0; i < 8; i++) {

		/* print two hex digits per byte */

		putchar("0123456789ABCDEF"[rom[i] >> 4]);
		putchar("0123456789ABCDEF"[rom[i] & 0x0F]);
	}

	putchar('\n');
}
