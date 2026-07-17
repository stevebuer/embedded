/*
 * Arm Cortex AX.25 Decoder
 */

#include <stdint.h>
#include "adc.h"
#include "goertzel.h"
#include "decoder.h"
#include "serial.h"
#include "tnc2.h"
#include "buttons.h"

#define HSI_FREQUENCY 8000000UL

void system_init(void);
void clock_init(void);
void gpio_init(void);
void led_init(void);
void led_toggle(void);

/* LED control (using built-in indicator, typically PB1 on STM32F030-NUCLEO) */

#define LED_PORT_BASE   0x48000400  /* GPIOB base */
#define GPIOB_MODER     (*(volatile uint32_t *)(LED_PORT_BASE + 0x00))
#define GPIOB_ODR       (*(volatile uint32_t *)(LED_PORT_BASE + 0x14))
#define LED_PIN         1           /* PB1 */

/* monitoring statistics */

volatile uint32_t sample_count = 0;
volatile uint32_t dma_overflow_count = 0;
volatile uint16_t last_buffer_pos = 0;

/* FSK decoder for AX.25 */

fsk_decoder_t fsk_decoder;

#define FSK_BLOCK_SIZE 64  /* Process 64 samples at a time */

/* bit and frame decoders */

bit_decoder_t bit_decoder;
frame_decoder_t frame_decoder;

/* system initialization called before main() */

void system_init(void)
{
    /* set HSION bit to enable HSI oscillator */

    uint32_t *rcc_cr = (uint32_t *) 0x40021000;

    *rcc_cr |= (1 << 0);
    
    /* wait for HSI ready */

    while (!(*rcc_cr & (1 << 1)));  /* Wait for HSIRDY */
}

/*  system clock: 8 Mhz HSI */

void clock_init(void)
{
    uint32_t *rcc_cfgr = (uint32_t *) 0x40021004;
    
    /* clear SWS, SW = 00 (HSI selected) */

    *rcc_cfgr &= ~3;
    *rcc_cfgr |= 0;
    
    /* enable peripheral clocks */

    uint32_t *rcc_ahbenr = (uint32_t *)0x40021014;

    *rcc_ahbenr |= (1 << 0);  /* GPIOAEN */
    *rcc_ahbenr |= (1 << 1);  /* GPIOBEN */
    *rcc_ahbenr |= (1 << 1);  /* DMA1EN */
    
    uint32_t *rcc_apb2enr = (uint32_t *)0x40021018;

    *rcc_apb2enr |= (1 << 9);  /* ADC1EN */
}

/* gpio init */

void gpio_init(void)
{
    /* PA0 is configured by adc_init() */
    /* Additional GPIO setup can go here */
}

/* led init */

void led_init(void)
{
	uint32_t *rcc_ahbenr = (uint32_t *)0x40021014;

	/* Enable GPIOB clock */

	*rcc_ahbenr |= (1 << 1);  
    
	/* Configure PB1 as output */

	/* config PB1 as output: clear mode bits, set as output (01b) */

	GPIOB_MODER &= ~(3 << 2);  
	GPIOB_MODER |= (1 << 2);   
}

/* rx indicator */

void led_toggle(void)
{
	GPIOB_ODR ^= (1 << LED_PIN);
}

/* ms delay (not calibrated) */

void delay_ms(uint32_t ms)
{
	volatile uint32_t count = ms * 1000;
	while (count--);
}

/* uart for debug output (todo) */

void debug_init(void)
{
	/* todo: Configure USART1 on PA9/PA10 */
}

/* main entry */

int main(void)
{
	clock_init();

	gpio_init();
	led_init();

	serial_init();
	serial_putstring("\r\n=== AX.25 Decoder Started ===\r\n");
    
	button_init();
	serial_putstring("Buttons ready\r\n");
    
	adc_init();
	dma_init();
    
	/* start continuous ADC sampling */

	adc_start();
    
	/* init FSK decoder (1200 Hz mark, 2200 Hz space) */

	fsk_init(&fsk_decoder, ADC_SAMPLE_RATE, FSK_BLOCK_SIZE);
    
	/* init bit and frame decoders */

	bit_decoder_init(&bit_decoder);

	frame_decoder_init(&frame_decoder);
    
	/* processing loop */

	uint32_t loop_counter = 0;
	uint16_t current_buffer_pos = 0;
	uint16_t previous_buffer_pos = 0;
	uint16_t block_samples = 0;
	uint16_t block_buffer[FSK_BLOCK_SIZE];
	uint8_t current_symbol = 0;
    
	while (1) {

		/* current DMA buffer position */

		current_buffer_pos = adc_get_buffer_position();
        
		/* new samples collected ? */

		if (current_buffer_pos != previous_buffer_pos) {

			/* new samples available for processing */

			uint16_t samples_available = 0;
			uint16_t read_pos = previous_buffer_pos;
            
			if (current_buffer_pos > previous_buffer_pos) {
                
				/* linear read from buffer */

                		samples_available = current_buffer_pos - previous_buffer_pos;

			} else if (current_buffer_pos < previous_buffer_pos) {

				/* buffer wrapped around */

				samples_available = (ADC_BUFFER_SIZE - previous_buffer_pos) + current_buffer_pos;
				dma_overflow_count++;
			}
            
			/* process new samples - accumulate for block processing */

			for (uint16_t i = 0; i < samples_available; i++) {

				uint16_t sample_idx = (read_pos + i) % ADC_BUFFER_SIZE;
				
				block_buffer[block_samples++] = adc_buffer[sample_idx];
                
				/* when block is full, process through FSK decoder */

				if (block_samples >= FSK_BLOCK_SIZE) {

					/* detect symbol (0 = space/2200Hz, 1 = mark/1200Hz) */

					current_symbol = fsk_process(&fsk_decoder, block_buffer, FSK_BLOCK_SIZE);
                    
					/* feed symbol to bit decoder (NRZI) */

					int16_t bit = bit_decoder_process(&bit_decoder, current_symbol);
                    
                    			/* if bit received, feed to frame decoder */

					if (bit >= 0) {

						int8_t frame_status = frame_decoder_process(&frame_decoder, (uint8_t) bit);
                        
						/* frame complete ? */

						if (frame_status > 0) {

							/* get the completed frame */
							
							uint16_t frame_len = 0;

							uint8_t *frame_data = frame_decoder_get_frame(&frame_decoder, &frame_len);
                            
							if (frame_data && frame_len > 0) {

								/* parse and output AX.25 frame */
						
								ax25_frame_t parsed_frame;

								if (ax25_parse_frame(frame_data, frame_len, &parsed_frame) == 0) {

									if (ax25_validate_fcs(&parsed_frame)) {

										/* frame is valid - output in TNC2 format */
						
										tnc2_print_frame(&parsed_frame);

									} else {
                                        
										/* CRC error */

										frame_decoder.frames_with_bad_fcs++;
                                    					}
                                				}
                            				}
                        			}
                    			}
                    
					block_samples = 0;
				}
			}
            
			sample_count += samples_available;
			previous_buffer_pos = current_buffer_pos;
		}
        
		/* poll buttons */

        	if (button_pressed_event(BUTTON_A))
            		serial_putstring("button A pressed\r\n");
        
        	if (button_pressed_event(BUTTON_B))
            		serial_putstring("button B pressed\r\n");
        
        	/* periodic activity - LED toggle every ~1 second */

        	loop_counter++;

		if (loop_counter >= 1000000) {

			led_toggle();
			loop_counter = 0;
            
			/* Optional: Output statistics */
			/* Debug output would go here */
			/* printf("Samples: %u, FSK Transitions: %u\n", sample_count, fsk_decoder.tone_changes); */
        	}

	}
    
	/* cleanup (not reached) */

	adc_stop();
    
	return 0;
}

/* fault handlers (minimal implementations) */

void hardfault_handler(void)
{
    /* rapid blink to indicate fault */

    while (1) {

        led_toggle();
        delay_ms(50);
    }
}

void nmi_handler(void)
{
    /* Non-maskable interrupt handler */
}

void svc_handler(void)
{
    /* Supervisor call handler */
}

void pendsv_handler(void)
{
    /* Pending SV handler */
}

void systick_handler(void)
{
    /* System tick handler */
}
