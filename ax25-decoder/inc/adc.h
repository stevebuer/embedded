#ifndef ADC_H
#define ADC_H

#include <stdint.h>

/* ADC constants */

#define ADC_BUFFER_SIZE 128
#define ADC_SAMPLE_RATE 9600
#define ADC_CHANNEL 0

/* ADC circular buffer */

extern volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
extern volatile uint16_t adc_write_index;

/* prototypes */

void adc_init(void);
void dma_init(void);
void adc_start(void);
void adc_stop(void);
uint16_t adc_get_sample(void);
uint16_t adc_get_dma_count(void);
uint16_t adc_get_buffer_position(void);

#endif
