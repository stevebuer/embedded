#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

/**
 * Serial UART Output (USART1 on PA9/PA10)
 * 115200 baud, 8N1
 */

/**
 * Initialize USART1
 */
void Serial_Init(void);

/**
 * Send single character
 */
void Serial_PutChar(char c);

/**
 * Send string (null-terminated)
 */
void Serial_PutString(const char *str);

/**
 * Send formatted output (minimal printf)
 * Supports: %s (string), %d (int), %x (hex), %c (char)
 */
void Serial_Printf(const char *fmt, ...);

/**
 * Check if USART1 is ready to transmit
 */
uint8_t Serial_Ready(void);

#endif /* SERIAL_H */
