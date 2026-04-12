/*
 * MCS-51 uart routines
 *
 * Steve Buer, N7MKO
 * Olympic College
 *
 */

#include <mcs51/8052.h>

/* RX and TX share interrupt 4 */

void serial_ISR(void) __interrupt(4) 
{
	if (RI) {

		/* clear RX flag */

		RI = 0;

		/* Read received byte */

		char c = SBUF;        

		/* Process the received byte here */
        
		SBUF = c; // echo loop
	}

	if (TI) {

		/* clear transmit flag */

		TI = 0;

		// Optional: handle transmit-complete events
	}
}

void init_uart()
{
	/* configure Timer 1 for baud rate generation */

	TMOD &= 0x0F;      // Clear T1 control bits
	TMOD |= 0x20;      // T1 Mode 2

	/* Load TH1 with the correct reload value */
	
	TH1 = 0xFD;        // 9600 baud @ 11.0592 MHz
	TL1 = 0xFD;

	/* Set SCON for serial mode 1 and REN = 1 (0b0101) */

	SCON = 0x50;

	/* Start Timer 1 */
    
	TR1 = 1; 

	/* enable interrupt */

	ES = 1;	
}

/* This must be implemented for SDCC libc stdio use */

int putchar(int c)
{
	/* wait until TI = 1, previous byte send completed */

	while (!TI);

	/* clear TI */

	TI = 0;

	/* MOV char to TX buf */

	SBUF = c;

	/* return transmitted char */

	return c;
}

// To Do: add configurable baud e.g. init_uart(9600);
