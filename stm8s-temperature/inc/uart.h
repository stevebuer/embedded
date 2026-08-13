#ifndef UART_H
#define UART_H

void uart_init(unsigned long baud);
void uart_putc(char c);
void uart_puts(const char* s);
void uart_puthex8(unsigned char value);
void uart_puthex32(unsigned long value);
unsigned char uart_rx_ready(void);
char uart_getc(void);

#endif
