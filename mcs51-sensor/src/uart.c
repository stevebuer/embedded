/*
 * MCS-51 uart routines
 *
 * Steve Buer, N7MKO
 * Olympic College
 *
 */

#include <mcs51/8052.h>

extern __bit cmd_flag;
extern char cmd_buf;

/* RX and TX share interrupt 4 */

void serial_ISR(void) __interrupt(4) 
{
	if (RI) {

		/* Read received byte */

		char c = SBUF;        
		
		/* clear RX flag */

		RI = 0;

		/* Process the received byte */

		cmd_buf = c;

		cmd_flag = 1;
	}
}

void init_uart()
{
	/* configure timer1 for mode: 2 */

	TMOD &= 0x0F;     
	TMOD |= 0x20;      

	/* TH1 holds reload value for 9600 baud @ 11.0592 Mhz, load TL1 initial value */
	
	TH1 = 0xFD;
	TL1 = 0xFD;

	/* set uartfor serial mode: 1 and REN = 1 (0b0101) */

	SCON = 0x50;

	/* start timer1 */
    
	TR1 = 1; 

	/* TI must be set for initial state or putchar() hangs */

	TI = 1;

	/* enable uart interrupt */

	ES = 1;	
}

/* putchar() must be implemented for sdcc libc stdio routines */

int putchar(int c)
{
	/* wait until TI = 1, previous byte send completed */

	while (!TI);

	/* clear TI */

	TI = 0;

	/* move char to tx buf */

	SBUF = c;

	/* return transmitted char */

	return c;
}

// To do: add configurable baud rate e.g. init_uart(9600);
