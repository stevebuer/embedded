#include "stm8s_conf.h"
#include "uart.h"

void uart_init(unsigned long baud)
{
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);
	UART1_DeInit();

	UART1_Init(baud,
		UART1_WORDLENGTH_8D,
		UART1_STOPBITS_1,
		UART1_PARITY_NO,
		UART1_SYNCMODE_CLOCK_DISABLE,
		UART1_MODE_TXRX_ENABLE);
}

void uart_putc(char c)
{
	UART1_SendData8((uint8_t)c);
	while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET) {
	}
}

void uart_puts(const char* s)
{
	while (*s) {
		uart_putc(*s++);
	}
}

void uart_puthex8(unsigned char value)
{
	const char hex[] = "0123456789ABCDEF";
	uart_putc(hex[(value >> 4) & 0x0F]);
	uart_putc(hex[value & 0x0F]);
}

void uart_puthex32(unsigned long value)
{
	uart_puthex8((unsigned char)(value >> 24));
	uart_puthex8((unsigned char)(value >> 16));
	uart_puthex8((unsigned char)(value >> 8));
	uart_puthex8((unsigned char)value);
}

unsigned char uart_rx_ready(void)
{
	return (UART1_GetFlagStatus(UART1_FLAG_RXNE) != RESET);
}

char uart_getc(void)
{
	return (char)UART1_ReceiveData8();
}
