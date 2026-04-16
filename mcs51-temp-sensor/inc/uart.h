/*
 * uart.h
 */

void init_uart(void);

void serial_ISR(void) __interrupt(4);
