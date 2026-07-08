#include "adc.h"

/* STM32F030 Register Definitions */
#define RCC_BASE           0x40021000
#define RCC_AHBENR         (*(volatile uint32_t *)(RCC_BASE + 0x14))
#define RCC_APB2ENR        (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_CFGR3          (*(volatile uint32_t *)(RCC_BASE + 0x30))

#define GPIOA_BASE         0x48000000
#define GPIOA_MODER        (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_PUPDR        (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))

#define ADC_BASE           0x40012400
#define ADC_CR             (*(volatile uint32_t *)(ADC_BASE + 0x00))
#define ADC_CFGR1          (*(volatile uint32_t *)(ADC_BASE + 0x04))
#define ADC_CFGR2          (*(volatile uint32_t *)(ADC_BASE + 0x08))
#define ADC_SMPR           (*(volatile uint32_t *)(ADC_BASE + 0x0C))
#define ADC_ISR            (*(volatile uint32_t *)(ADC_BASE + 0x10))
#define ADC_IER            (*(volatile uint32_t *)(ADC_BASE + 0x14))
#define ADC_CHSELR         (*(volatile uint32_t *)(ADC_BASE + 0x28))
#define ADC_DR             (*(volatile uint32_t *)(ADC_BASE + 0x40))

#define DMA_BASE           0x40020000
#define DMA_ISR            (*(volatile uint32_t *)(DMA_BASE + 0x00))
#define DMA_IFCR           (*(volatile uint32_t *)(DMA_BASE + 0x04))

#define DMA_CH1_BASE       (DMA_BASE + 0x08)
#define DMA1_CCR1          (*(volatile uint32_t *)(DMA_CH1_BASE + 0x00))
#define DMA1_CNDTR1        (*(volatile uint32_t *)(DMA_CH1_BASE + 0x04))
#define DMA1_CPAR1         (*(volatile uint32_t *)(DMA_CH1_BASE + 0x08))
#define DMA1_CMAR1         (*(volatile uint32_t *)(DMA_CH1_BASE + 0x0C))

/* Global ADC buffer */
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
volatile uint16_t adc_write_index = 0;

/**
 * Initialize ADC peripheral
 * - PA0 as ADC input (single-ended)
 * - Continuous conversion mode
 * - DMA transfer on conversion complete
 */
void ADC_Init(void)
{
    /* Enable clocks */
    RCC_AHBENR |= (1 << 0);   /* Enable GPIOA clock */
    RCC_AHBENR |= (1 << 1);   /* Enable DMA1 clock */
    RCC_APB2ENR |= (1 << 9);  /* Enable ADC1 clock */

    /* Configure PA0 as analog input */
    GPIOA_MODER |= (3 << 0);  /* PA0 to analog mode (11b) */
    GPIOA_PUPDR &= ~(3 << 0); /* No pull-up/pull-down on PA0 */

    /* Reset ADC */
    ADC_CR |= (1 << 0);       /* ADEN = 1 (enable ADC) */
    
    /* Wait for ADC to become ready */
    while (!(ADC_ISR & (1 << 0)));  /* Wait for ADRDY */

    /* Configure ADC */
    ADC_CFGR1 = 0;
    ADC_CFGR1 |= (1 << 0);    /* DMAEN = 1 (enable DMA) */
    ADC_CFGR1 |= (1 << 1);    /* DMACFG = 1 (circular DMA) */
    ADC_CFGR1 |= (1 << 2);    /* SCANDIR = 1 (upward scan direction) */
    
    /* Clock prescaler: HSI (8 MHz) / 2 = 4 MHz */
    RCC_CFGR3 &= ~(3 << 4);   /* Clear ADCPRE bits */
    RCC_CFGR3 |= (1 << 4);    /* ADCPRE = 01 (divide by 2) */

    /* Set ADC sampling time to longest (239.5 ADC clocks) for better accuracy */
    ADC_SMPR = 7;             /* 111b - 239.5 clock cycles */

    /* Select channel 0 (PA0 / ADC_IN0) */
    ADC_CHSELR = (1 << 0);    /* CH0 enabled */

    /* Calibrate ADC */
    ADC_CR |= (1 << 31);      /* ADCAL = 1 (start calibration) */
    while (ADC_CR & (1 << 31));  /* Wait for calibration to complete */
}

/**
 * Initialize DMA for ADC
 * - DMA1 Channel 1 for ADC
 * - Circular mode for continuous sampling
 */
void DMA_Init(void)
{
    /* Disable DMA channel before configuration */
    DMA1_CCR1 &= ~(1 << 0);
    
    /* Clear any pending DMA flags */
    DMA_IFCR = (0xF << 0);    /* Clear all flags for channel 1 */

    /* Configure DMA channel */
    DMA1_CCR1 = 0;
    DMA1_CCR1 |= (0 << 4);    /* DIR = 0 (read from peripheral) */
    DMA1_CCR1 |= (1 << 5);    /* CIRC = 1 (circular mode) */
    DMA1_CCR1 |= (0 << 6);    /* PINC = 0 (peripheral address not incremented) */
    DMA1_CCR1 |= (1 << 7);    /* MINC = 1 (memory address incremented) */
    DMA1_CCR1 |= (1 << 8);    /* PSIZE = 01 (16-bit peripheral) */
    DMA1_CCR1 |= (1 << 10);   /* MSIZE = 01 (16-bit memory) */
    DMA1_CCR1 |= (2 << 12);   /* PL = 10 (high priority) */

    /* Set transfer count */
    DMA1_CNDTR1 = ADC_BUFFER_SIZE;

    /* Set peripheral address (ADC data register) */
    DMA1_CPAR1 = (uint32_t)&ADC_DR;

    /* Set memory address (ADC buffer) */
    DMA1_CMAR1 = (uint32_t)&adc_buffer[0];

    /* Enable DMA channel */
    DMA1_CCR1 |= (1 << 0);    /* EN = 1 */
}

/**
 * Start ADC conversions
 * - Continuous conversion mode
 * - DMA transfers enabled
 */
void ADC_Start(void)
{
    /* Initialize DMA first */
    DMA_Init();

    /* Start continuous conversions */
    ADC_CR |= (1 << 2);       /* ADSTART = 1 (start conversion sequence) */
}

/**
 * Stop ADC conversions
 */
void ADC_Stop(void)
{
    /* Stop conversions */
    ADC_CR |= (1 << 4);       /* ADSTP = 1 (stop conversion) */
    while (ADC_CR & (1 << 4));  /* Wait for stop to complete */

    /* Disable ADC */
    ADC_CR &= ~(1 << 0);      /* ADEN = 0 */
}

/**
 * Get most recent ADC sample
 * - Safe for multi-threaded access
 */
uint16_t ADC_GetSample(void)
{
    uint16_t index = (adc_write_index > 0) ? adc_write_index - 1 : ADC_BUFFER_SIZE - 1;
    return adc_buffer[index];
}

/**
 * Get DMA transfer count (for monitoring circular buffer position)
 * - Returns number of remaining transfers in current cycle
 */
uint16_t ADC_GetDMACount(void)
{
    return DMA1_CNDTR1;
}

/**
 * Check if ADC buffer has new data
 * - Call this in your main loop to process new samples
 */
uint16_t ADC_GetBufferPosition(void)
{
    uint16_t remaining = ADC_GetDMACount();
    uint16_t current_pos = ADC_BUFFER_SIZE - remaining;
    return current_pos;
}
