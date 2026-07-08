#include "goertzel.h"
#include <math.h>

#define PI 3.14159265358979323846f

/**
 * Initialize Goertzel filter for frequency detection
 */
void Goertzel_Init(goertzel_t *g, float target_freq, uint32_t sample_rate, 
                   uint16_t block_size)
{
    g->target_frequency = target_freq;
    g->sample_rate = sample_rate;
    g->block_size = block_size;
    
    /* Calculate normalized frequency (0 to 1) */
    float k = (float)block_size * target_freq / (float)sample_rate;
    
    /* Goertzel coefficient: 2 * cos(2 * pi * k / N) */
    float omega = 2.0f * PI * k / (float)block_size;
    g->coeff = 2.0f * cosf(omega);
    
    /* Initialize IIR state */
    g->Q0 = 0.0f;
    g->Q1 = 0.0f;
    g->Q2 = 0.0f;
    
    /* Default threshold (tunable) */
    g->threshold = 1000.0f;
    
    /* Statistics */
    g->blocks_processed = 0;
    g->threshold_crossings = 0;
    g->magnitude = 0.0f;
    g->power = 0.0f;
}

/**
 * Process a block of samples
 * Implements the Goertzel IIR filter followed by magnitude calculation
 */
float Goertzel_Process(goertzel_t *g, const uint16_t *samples, uint16_t count)
{
    /* Run IIR filter on all samples */
    for (uint16_t i = 0; i < count; i++)
    {
        /* Convert ADC sample (0-4095 for 12-bit ADC, centered) */
        float sample = (float)samples[i] - 2048.0f;
        
        /* Goertzel IIR filter stage */
        g->Q0 = g->coeff * g->Q1 - g->Q2 + sample;
        g->Q2 = g->Q1;
        g->Q1 = g->Q0;
    }
    
    /* Calculate magnitude at target frequency */
    float k = (float)g->block_size * g->target_frequency / (float)g->sample_rate;
    float omega = 2.0f * PI * k / (float)g->block_size;
    
    /* DFT components */
    float real = g->Q1 - g->Q2 * cosf(omega);
    float imag = g->Q2 * sinf(omega);
    
    /* Magnitude: sqrt(real^2 + imag^2) */
    g->magnitude = sqrtf(real * real + imag * imag);
    
    /* Power: magnitude^2 */
    g->power = g->magnitude * g->magnitude;
    
    /* Update statistics */
    g->blocks_processed++;
    
    /* Check threshold crossing */
    if (g->power > g->threshold)
    {
        g->threshold_crossings++;
    }
    
    return g->magnitude;
}

/**
 * Reset filter state between blocks
 */
void Goertzel_Reset(goertzel_t *g)
{
    g->Q0 = 0.0f;
    g->Q1 = 0.0f;
    g->Q2 = 0.0f;
}

/**
 * Check if tone is detected above threshold
 */
uint8_t Goertzel_ToneDetected(goertzel_t *g)
{
    return (g->power > g->threshold) ? 1 : 0;
}

/**
 * Set detection threshold
 */
void Goertzel_SetThreshold(goertzel_t *g, float threshold)
{
    g->threshold = threshold;
}

/**
 * Get current power measurement
 */
float Goertzel_GetPower(goertzel_t *g)
{
    return g->power;
}

/**
 * Get current magnitude measurement
 */
float Goertzel_GetMagnitude(goertzel_t *g)
{
    return g->magnitude;
}

/**
 * Example: Dual-tone detector for AX.25 (1200 Hz mark, 2200 Hz space)
 */

/**
 * Initialize FSK decoder with dual-tone detection
 */
void FSK_Init(fsk_decoder_t *fsk, uint32_t sample_rate, uint16_t block_size)
{
    /* AX.25 Bell 202 FSK: 1200 Hz (mark), 2200 Hz (space) */
    Goertzel_Init(&fsk->mark, 1200.0f, sample_rate, block_size);
    Goertzel_Init(&fsk->space, 2200.0f, sample_rate, block_size);
    
    /* Set thresholds (tunable based on SNR) */
    Goertzel_SetThreshold(&fsk->mark, 5000.0f);
    Goertzel_SetThreshold(&fsk->space, 5000.0f);
    
    fsk->current_tone = 0;
    fsk->tone_changes = 0;
}

/**
 * Process sample block for FSK detection
 * Returns detected symbol (0 = space, 1 = mark)
 */
uint8_t FSK_Process(fsk_decoder_t *fsk, const uint16_t *samples, uint16_t count)
{
    /* Process both tones */
    float mark_mag = Goertzel_Process(&fsk->mark, samples, count);
    Goertzel_Reset(&fsk->mark);
    
    Goertzel_Reset(&fsk->space);
    float space_mag = Goertzel_Process(&fsk->space, samples, count);
    Goertzel_Reset(&fsk->space);
    
    /* Determine which tone is stronger */
    uint8_t detected_tone = (mark_mag > space_mag) ? 1 : 0;
    
    /* Detect tone transition */
    if (detected_tone != fsk->current_tone)
    {
        fsk->tone_changes++;
        fsk->current_tone = detected_tone;
    }
    
    return fsk->current_tone;
}
