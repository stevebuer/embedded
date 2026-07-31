/*
 * ADC and DMA Stub for Host Testing
 * 
 * This is a stub implementation of the ADC driver for host-based testing.
 * These functions are not used during WAV file testing since samples are
 * fed directly to the decoder, but they're here for compatibility.
 */

#include <stdint.h>
#include <string.h>
#include "../inc/adc.h"

/* ADC circular buffer (dummy, not used in host test) */
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE] = {0};
volatile uint16_t adc_write_index = 0;

/**
 * Initialize ADC (stub)
 */
void adc_init(void)
{
	/* Nothing to do for host */
	memset((void *)adc_buffer, 0, sizeof(adc_buffer));
	adc_write_index = 0;
}

/**
 * Initialize DMA (stub)
 */
void dma_init(void)
{
	/* Nothing to do for host */
}

/**
 * Start ADC sampling (stub)
 */
void adc_start(void)
{
	/* Nothing to do for host */
}

/**
 * Stop ADC sampling (stub)
 */
void adc_stop(void)
{
	/* Nothing to do for host */
}

/**
 * Get single ADC sample (stub)
 */
uint16_t adc_get_sample(void)
{
	/* Return dummy value */
	return 0;
}

/**
 * Get DMA transfer count (stub)
 */
uint16_t adc_get_dma_count(void)
{
	/* Return dummy value */
	return 0;
}

/**
 * Get current buffer position (stub)
 */
uint16_t adc_get_buffer_position(void)
{
	/* Return dummy value */
	return 0;
}
