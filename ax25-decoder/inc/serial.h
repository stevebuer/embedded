/*
 * Serial UART Output (USART1 on PA9/PA10)
 * 115200 baud, 8N1
 *
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

void serial_init(void);
void serial_putchar(char c);
void serial_putstring(const char *str);
void serial_printf(const char *fmt, ...);
uint8_t serial_ready(void);

#endif /* SERIAL_H */
