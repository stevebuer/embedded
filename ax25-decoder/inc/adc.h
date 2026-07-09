#ifndef ADC_H
#define ADC_H

#include <stdint.h>

/* ADC Configuration Constants */
#define ADC_BUFFER_SIZE 128         /* Reduced from 256 to fit in RAM */
#define ADC_SAMPLE_RATE 9600  /* Hz - typical for AX.25 audio sampling */
#define ADC_CHANNEL 0         /* ADC input channel (PA0 / ADC_IN0) */

/* ADC buffer (circular) */
extern volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
extern volatile uint16_t adc_write_index;

/* Function prototypes */
void ADC_Init(void);
void DMA_Init(void);
void ADC_Start(void);
void ADC_Stop(void);
uint16_t ADC_GetSample(void);
uint16_t ADC_GetDMACount(void);
uint16_t ADC_GetBufferPosition(void);

#endif /* ADC_H */
