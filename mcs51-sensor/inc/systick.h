/*
 * systick.h
 */

void init_systick(void);

uint16_t millis(void);
uint16_t systick_read(void);

void delay_ms(uint16_t ms); 

void timer0_ISR(void) __interrupt(1);
