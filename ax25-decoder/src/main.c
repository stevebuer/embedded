#include <stdint.h>
#include "adc.h"
#include "goertzel.h"
#include "decoder.h"
#include "serial.h"
#include "tnc2.h"
#include "buttons.h"

/* System clock configuration */
#define HSI_FREQUENCY 8000000UL

/* Function prototypes */
void SystemInit(void);
void Clock_Init(void);
void GPIO_Init(void);
void LED_Init(void);
void LED_Toggle(void);

/* LED control (using built-in indicator, typically PB1 on STM32F030-NUCLEO) */
#define LED_PORT_BASE   0x48000400  /* GPIOB base */
#define GPIOB_MODER     (*(volatile uint32_t *)(LED_PORT_BASE + 0x00))
#define GPIOB_ODR       (*(volatile uint32_t *)(LED_PORT_BASE + 0x14))
#define LED_PIN         1           /* PB1 */

/* Statistics for monitoring */
volatile uint32_t sample_count = 0;
volatile uint32_t dma_overflow_count = 0;
volatile uint16_t last_buffer_pos = 0;

/* FSK Decoder for AX.25 */
fsk_decoder_t fsk_decoder;
#define FSK_BLOCK_SIZE 64  /* Process 64 samples at a time */

/* Bit and Frame Decoders */
bit_decoder_t bit_decoder;
frame_decoder_t frame_decoder;

/**
 * System initialization
 * Called by startup code before main()
 */
void SystemInit(void)
{
    /* Set HSION bit to enable HSI oscillator */
    uint32_t *rcc_cr = (uint32_t *)0x40021000;
    *rcc_cr |= (1 << 0);  /* HSION = 1 */
    
    /* Wait for HSI ready */
    while (!(*rcc_cr & (1 << 1)));  /* Wait for HSIRDY */
}

/**
 * Clock initialization
 * HSI 8 MHz as system clock
 */
void Clock_Init(void)
{
    uint32_t *rcc_cfgr = (uint32_t *)0x40021004;
    
    /* Select HSI as system clock source */
    *rcc_cfgr &= ~3;        /* Clear SWS bits */
    *rcc_cfgr |= 0;         /* SW = 00 (HSI selected) */
    
    /* Enable all necessary peripheral clocks */
    uint32_t *rcc_ahbenr = (uint32_t *)0x40021014;
    *rcc_ahbenr |= (1 << 0);  /* GPIOAEN */
    *rcc_ahbenr |= (1 << 1);  /* GPIOBEN */
    *rcc_ahbenr |= (1 << 1);  /* DMA1EN */
    
    uint32_t *rcc_apb2enr = (uint32_t *)0x40021018;
    *rcc_apb2enr |= (1 << 9);  /* ADC1EN */
}

/**
 * GPIO initialization
 */
void GPIO_Init(void)
{
    /* PA0 is configured by ADC_Init() */
    /* Additional GPIO setup can go here */
}

/**
 * LED initialization - visual feedback
 */
void LED_Init(void)
{
    uint32_t *rcc_ahbenr = (uint32_t *)0x40021014;
    *rcc_ahbenr |= (1 << 1);  /* Enable GPIOB clock */
    
    /* Configure PB1 as output */
    GPIOB_MODER &= ~(3 << 2);  /* Clear PB1 mode bits */
    GPIOB_MODER |= (1 << 2);   /* Set PB1 to output mode (01b) */
}

/**
 * Toggle LED for visual indication
 */
void LED_Toggle(void)
{
    GPIOB_ODR ^= (1 << LED_PIN);
}

/**
 * Simple delay in milliseconds
 * Approximate, not precise
 */
void Delay_ms(uint32_t ms)
{
    volatile uint32_t count = ms * 1000;
    while (count--);
}

/**
 * UART for debug output (optional future enhancement)
 * Placeholder for future logging
 */
void Debug_Init(void)
{
    /* TODO: Configure USART1 on PA9/PA10 */
}

/**
 * Main entry point
 */
int main(void)
{
    /* Initialize system */
    Clock_Init();
    GPIO_Init();
    LED_Init();
    
    /* Initialize serial output */
    Serial_Init();
    Serial_PutString("\r\n=== AX.25 Decoder Started ===\r\n");
    
    /* Initialize button inputs */
    Button_Init();
    Serial_PutString("Buttons ready\r\n");
    
    /* Initialize ADC and DMA */
    ADC_Init();
    DMA_Init();
    
    /* Start continuous ADC sampling */
    ADC_Start();
    
    /* Initialize FSK decoder (1200 Hz mark, 2200 Hz space) */
    FSK_Init(&fsk_decoder, ADC_SAMPLE_RATE, FSK_BLOCK_SIZE);
    
    /* Initialize bit and frame decoders */
    BitDecoder_Init(&bit_decoder);
    FrameDecoder_Init(&frame_decoder);
    
    /* Main processing loop */
    uint32_t loop_counter = 0;
    uint16_t current_buffer_pos = 0;
    uint16_t previous_buffer_pos = 0;
    uint16_t block_samples = 0;
    uint16_t block_buffer[FSK_BLOCK_SIZE];
    uint8_t current_symbol = 0;
    
    while (1)
    {
        /* Get current DMA buffer position */
        current_buffer_pos = ADC_GetBufferPosition();
        
        /* Check if new samples have been collected */
        if (current_buffer_pos != previous_buffer_pos)
        {
            /* New samples available for processing */
            uint16_t samples_available = 0;
            uint16_t read_pos = previous_buffer_pos;
            
            if (current_buffer_pos > previous_buffer_pos)
            {
                /* Linear read from buffer */
                samples_available = current_buffer_pos - previous_buffer_pos;
            }
            else if (current_buffer_pos < previous_buffer_pos)
            {
                /* Buffer wrapped around */
                samples_available = (ADC_BUFFER_SIZE - previous_buffer_pos) + current_buffer_pos;
                dma_overflow_count++;
            }
            
            /* Process new samples - accumulate for block processing */
            for (uint16_t i = 0; i < samples_available; i++)
            {
                uint16_t sample_idx = (read_pos + i) % ADC_BUFFER_SIZE;
                block_buffer[block_samples++] = adc_buffer[sample_idx];
                
                /* When block is full, process through FSK decoder */
                if (block_samples >= FSK_BLOCK_SIZE)
                {
                    /* Detect symbol (0=space/2200Hz, 1=mark/1200Hz) */
                    current_symbol = FSK_Process(&fsk_decoder, block_buffer, FSK_BLOCK_SIZE);
                    
                    /* Feed symbol to bit decoder (NRZI) */
                    int16_t bit = BitDecoder_Process(&bit_decoder, current_symbol);
                    
                    /* If we got a bit, feed to frame decoder */
                    if (bit >= 0)
                    {
                        int8_t frame_status = FrameDecoder_Process(&frame_decoder, (uint8_t)bit);
                        
                        /* Check if frame is complete */
                        if (frame_status > 0)
                        {
                            /* Get the completed frame */
                            uint16_t frame_len = 0;
                            uint8_t *frame_data = FrameDecoder_GetFrame(&frame_decoder, &frame_len);
                            
                            if (frame_data && frame_len > 0)
                            {
                                /* Parse and output AX.25 frame */
                                ax25_frame_t parsed_frame;
                                if (AX25_ParseFrame(frame_data, frame_len, &parsed_frame) == 0)
                                {
                                    if (AX25_ValidateFCS(&parsed_frame))
                                    {
                                        /* Frame is valid - output in TNC2 format */
                                        TNC2_PrintFrame(&parsed_frame);
                                    }
                                    else
                                    {
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
        
        /* Poll buttons for input */
        if (Button_PressedEvent(BUTTON_A))
        {
            Serial_PutString("Button A pressed\r\n");
        }
        
        if (Button_PressedEvent(BUTTON_B))
        {
            Serial_PutString("Button B pressed\r\n");
        }
        
        /* Periodic activity - LED toggle every ~1 second */
        loop_counter++;
        if (loop_counter >= 1000000)
        {
            LED_Toggle();
            loop_counter = 0;
            
            /* Optional: Output statistics */
            /* Debug output would go here */
            /* printf("Samples: %u, FSK Transitions: %u\n", sample_count, fsk_decoder.tone_changes); */
        }
    }
    
    /* Cleanup (never reached in embedded system) */
    ADC_Stop();
    
    return 0;
}

/**
 * Fault handlers (minimal implementations)
 */
void HardFault_Handler(void)
{
    /* Blink LED rapidly to indicate fault */
    while (1)
    {
        LED_Toggle();
        Delay_ms(50);
    }
}

void NMI_Handler(void)
{
    /* Non-maskable interrupt handler */
}

void SVC_Handler(void)
{
    /* Supervisor call handler */
}

void PendSV_Handler(void)
{
    /* Pending SV handler */
}

void SysTick_Handler(void)
{
    /* System tick handler */
}
