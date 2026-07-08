#include "serial.h"
#include <stdarg.h>

/* STM32F030 USART1 Register Definitions */
#define RCC_BASE           0x40021000
#define RCC_AHBENR         (*(volatile uint32_t *)(RCC_BASE + 0x14))
#define RCC_APB2ENR        (*(volatile uint32_t *)(RCC_BASE + 0x18))

#define GPIOA_BASE         0x48000000
#define GPIOA_MODER        (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_AFRH         (*(volatile uint32_t *)(GPIOA_BASE + 0x24))

#define USART1_BASE        0x40013800
#define USART1_CR1         (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART1_CR2         (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART1_CR3         (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART1_BRR         (*(volatile uint32_t *)(USART1_BASE + 0x0C))
#define USART1_TDR         (*(volatile uint32_t *)(USART1_BASE + 0x28))
#define USART1_ISR         (*(volatile uint32_t *)(USART1_BASE + 0x1C))

/**
 * Initialize USART1
 * PA9 = TX, PA10 = RX
 * 115200 baud, 8 bits, no parity, 1 stop bit
 */
void Serial_Init(void)
{
    /* Enable GPIO and USART1 clocks */
    RCC_AHBENR |= (1 << 0);   /* Enable GPIOA clock */
    RCC_APB2ENR |= (1 << 14); /* Enable USART1 clock */
    
    /* Configure PA9 (TX) and PA10 (RX) as alternate function */
    GPIOA_MODER &= ~(3 << 18);
    GPIOA_MODER |= (2 << 18);
    GPIOA_MODER &= ~(3 << 20);
    GPIOA_MODER |= (2 << 20);
    
    /* Set alternate function AF1 */
    GPIOA_AFRH &= ~(0xF << 4);
    GPIOA_AFRH |= (1 << 4);
    GPIOA_AFRH &= ~(0xF << 8);
    GPIOA_AFRH |= (1 << 8);
    
    /* Disable while configuring */
    USART1_CR1 &= ~(1 << 0);
    
    /* Baud rate: 9600 at 8MHz = BRR 833 */
    USART1_BRR = 833;
    
    /* Enable TX/RX */
    USART1_CR1 = 0;
    USART1_CR1 |= (1 << 3);   /* TE */
    USART1_CR1 |= (1 << 2);   /* RE */
    USART1_CR1 |= (1 << 0);   /* UE */
    USART1_CR2 = 0;
    USART1_CR3 = 0;
}

/**
 * Check if ready to transmit
 */
uint8_t Serial_Ready(void)
{
    return (USART1_ISR & (1 << 7)) ? 1 : 0;
}

/**
 * Send single character
 */
void Serial_PutChar(char c)
{
    while (!Serial_Ready());
    USART1_TDR = (uint8_t)c;
}

/**
 * Send string
 */
void Serial_PutString(const char *str)
{
    if (!str)
        return;
    
    while (*str)
    {
        if (*str == '\n')
            Serial_PutChar('\r');
        Serial_PutChar(*str++);
    }
}

/**
 * Convert integer to string (no dependency on snprintf)
 */
static int IntToStr(int value, char *buf, int base)
{
    const char digits[] = "0123456789abcdef";
    char temp[32];
    int len = 0;
    int neg = 0;
    
    if (value < 0 && base == 10)
    {
        neg = 1;
        value = -value;
    }
    
    if (value == 0)
    {
        buf[0] = '0';
        return 1;
    }
    
    while (value > 0)
    {
        temp[len++] = digits[value % base];
        value /= base;
    }
    
    int i = 0;
    if (neg)
    {
        buf[i++] = '-';
    }
    
    for (int j = len - 1; j >= 0; j--)
        buf[i++] = temp[j];
    
    return i;
}

/**
 * Minimal printf: %s, %d, %x, %c
 */
void Serial_Printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    
    while (*fmt)
    {
        if (*fmt == '%' && *(fmt + 1))
        {
            fmt++;
            switch (*fmt)
            {
                case 's':
                {
                    const char *s = va_arg(args, const char *);
                    if (s)
                        Serial_PutString(s);
                    break;
                }
                
                case 'd':
                {
                    int d = va_arg(args, int);
                    char buf[16];
                    int len = IntToStr(d, buf, 10);
                    for (int i = 0; i < len; i++)
                        Serial_PutChar(buf[i]);
                    break;
                }
                
                case 'x':
                {
                    unsigned int x = va_arg(args, unsigned int);
                    char buf[16];
                    int len = IntToStr(x, buf, 16);
                    for (int i = 0; i < len; i++)
                        Serial_PutChar(buf[i]);
                    break;
                }
                
                case 'c':
                {
                    int c = va_arg(args, int);
                    Serial_PutChar((char)c);
                    break;
                }
            }
            fmt++;
        }
        else
        {
            Serial_PutChar(*fmt++);
        }
    }
    
    va_end(args);
}
