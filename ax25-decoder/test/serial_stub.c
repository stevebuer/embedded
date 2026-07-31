/*
 * Serial UART Stub for Host Testing
 * 
 * This is a stub implementation of the serial driver for host-based testing.
 * Output goes to stdout instead of hardware UART.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "../inc/serial.h"

/**
 * Initialize USART (stub)
 */
void serial_init(void)
{
	/* Nothing to do for host */
}

/**
 * Send single character to stdout
 */
void serial_putchar(char c)
{
	putchar(c);
	fflush(stdout);
}

/**
 * Send string to stdout
 */
void serial_putstring(const char *str)
{
	while (*str) {
		serial_putchar(*str++);
	}
}

/**
 * Printf-style formatted output
 */
void serial_printf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	fflush(stdout);
}

/**
 * Check if serial port is ready (always true for host)
 */
uint8_t serial_ready(void)
{
	return 1;
}
