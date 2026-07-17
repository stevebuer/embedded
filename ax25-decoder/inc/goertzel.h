#ifndef GOERTZEL_H
#define GOERTZEL_H

#include <stdint.h>

/**
 * Goertzel Filter
 * 
 * Efficient single-frequency tone detector using DFT calculation
 * Ideal for AX.25 audio decoding (1200 Hz mark, 2200 Hz space)
 * 
 * References:
 * - Goertzel Algorithm (1958)
 * - Used in DTMF detection, FSK modulation
 */

typedef struct {

    /* Configuration */

    float target_frequency;      /* Frequency to detect (Hz) */
    uint32_t sample_rate;        /* ADC sample rate (Hz) */
    uint16_t block_size;         /* Samples per analysis block */
    
    /* Coefficients */

    float coeff;                 /* 2 * cos(2 * pi * k / N) */
    float Q0, Q1, Q2;            /* IIR filter state */
    
    /* Results */

    float magnitude;             /* Magnitude of detected frequency */
    float power;                 /* Power (magnitude^2) */
    float threshold;             /* Detection threshold */
    
    /* Statistics */

    uint32_t blocks_processed;
    uint32_t threshold_crossings;

} goertzel_t;

/**
 * Initialize Goertzel filter
 * 
 * @param g Goertzel filter instance
 * @param target_freq Frequency to detect (Hz)
 * @param sample_rate Sample rate (Hz)
 * @param block_size Samples per block (power of 2 recommended)
 */

void goertzel_init(goertzel_t *g, float target_freq, uint32_t sample_rate, uint16_t block_size);

/**
 * Process a block of samples and calculate magnitude at target frequency
 * Call after collecting block_size samples
 * 
 * @param g Goertzel filter instance
 * @param samples Input sample buffer
 * @param count Number of samples (should equal block_size)
 * @return Magnitude at target frequency
 */

float goertzel_process(goertzel_t *g, const uint16_t *samples, uint16_t count);

/**
 * Reset filter state (zeros out IIR state)
 * Call between blocks to start fresh
 * 
 * @param g Goertzel filter instance
 */

void goertzel_reset(goertzel_t *g);

/**
 * Check if magnitude exceeds threshold (tone detected)
 * 
 * @param g Goertzel filter instance
 * @return 1 if tone detected, 0 otherwise
 */

uint8_t goertzel_tone_detected(goertzel_t *g);

/**
 * Set detection threshold
 * 
 * @param g Goertzel filter instance
 * @param threshold Power threshold for tone detection
 */

void goertzel_set_threshold(goertzel_t *g, float threshold);

/**
 * Get current power measurement
 * 
 * @param g Goertzel filter instance
 * @return Power at target frequency
 */

float goertzel_get_power(goertzel_t *g);

/**
 * Get current magnitude measurement
 * 
 * @param g Goertzel filter instance
 * @return Magnitude at target frequency
 */

float goertzel_get_magnitude(goertzel_t *g);

/**
 * FSK Decoder - Dual-tone frequency shift keying
 * Detects Bell 202 FSK: 1200 Hz (mark), 2200 Hz (space)
 */

typedef struct {

    goertzel_t mark;           /* 1200 Hz detector */
    goertzel_t space;          /* 2200 Hz detector */
    uint8_t current_tone;      /* 0 = space, 1 = mark */
    uint32_t tone_changes;     /* Transition counter */

} fsk_decoder_t;

/**
 * Initialize FSK decoder with dual-tone detection
 * 
 * @param fsk FSK decoder instance
 * @param sample_rate Sample rate (Hz)
 * @param block_size Samples per block
 */

void fsk_init(fsk_decoder_t *fsk, uint32_t sample_rate, uint16_t block_size);

/**
 * Process sample block for FSK detection
 * Returns detected symbol (0 = space/2200Hz, 1 = mark/1200Hz)
 * 
 * @param fsk FSK decoder instance
 * @param samples Input samples
 * @param count Sample count
 * @return Current detected symbol
 */

uint8_t fsk_process(fsk_decoder_t *fsk, const uint16_t *samples, uint16_t count);

#endif /* GOERTZEL_H */
