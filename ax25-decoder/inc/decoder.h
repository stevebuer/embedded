#ifndef DECODER_H
#define DECODER_H

#include <stdint.h>

/**
 * AX.25 Bit and Frame Decoder
 * 
 * Converts FSK symbols -> bits (NRZI) -> frames
 * Handles frame synchronization, bit stuffing, and FCS validation
 */

/* AX.25 Frame Constants */
#define AX25_FLAG           0x7E         /* Frame flag byte */
#define AX25_ESCAPE         0x7D         /* Escape byte for transparency */
#define AX25_MAX_FRAME_LEN  160          /* Max AX.25 frame length (reduced) */
#define AX25_MIN_FRAME_LEN  14           /* Minimum frame (flags + SSID + control + PID) */

/* Decoder states */
typedef enum {
    STATE_SYNCING,          /* Searching for frame sync */
    STATE_IN_FRAME,         /* Collecting frame data */
    STATE_FRAME_COMPLETE,   /* Frame ready for processing */
    STATE_ERROR             /* Decode error */
} decoder_state_t;

/**
 * Bit-level decoder
 * Converts FSK symbols to bits using NRZI decoding
 */
typedef struct {
    /* Symbol history for NRZI */
    uint8_t last_symbol;
    uint8_t last_bit;
    
    /* Bit accumulator */
    uint8_t bit_buffer;
    uint8_t bit_count;
    
    /* Statistics */
    uint32_t bits_decoded;
    uint32_t bit_errors;
} bit_decoder_t;

/**
 * Frame-level decoder
 * Handles frame synchronization, bit stuffing, and FCS
 */
typedef struct {
    /* State machine */
    decoder_state_t state;
    
    /* Frame buffer */
    uint8_t frame_buffer[AX25_MAX_FRAME_LEN];
    uint16_t frame_length;
    uint16_t frame_count;
    
    /* Bit accumulation */
    uint8_t bit_buffer;         /* Accumulate bits into byte */
    uint8_t bit_count;          /* Count of bits in buffer */
    
    /* Bit stuffing tracker */
    uint8_t stuff_count;        /* Count of consecutive 1s */
    uint8_t frame_flags_seen;   /* Number of flag bytes seen */
    
    /* CRC/FCS */
    uint16_t fcs;               /* Frame check sequence */
    uint16_t fcs_valid;         /* FCS validity flag */
    
    /* Sync and timing */
    uint32_t bits_since_flag;   /* Bit counter for sync */
    uint32_t frames_received;
    uint32_t frames_with_bad_fcs;
    uint32_t sync_losses;
} frame_decoder_t;

/**
 * Parsed AX.25 frame (minimal structure)
 */
typedef struct {
    /* Addresses */
    uint8_t dest_call[7];       /* Destination callsign + SSID */
    uint8_t src_call[7];        /* Source callsign + SSID */
    uint8_t dest_ssid;
    uint8_t src_ssid;
    
    /* Control fields */
    uint8_t control;            /* Control byte */
    uint8_t pid;                /* Protocol identifier */
    
    /* Data */
    uint8_t *payload;           /* Pointer to payload data */
    uint16_t payload_len;
    
    /* Validation */
    uint16_t fcs;               /* Received FCS */
    uint16_t fcs_calculated;    /* Calculated FCS */
    uint8_t fcs_valid;          /* FCS match flag */
} ax25_frame_t;

/**
 * Initialize bit decoder
 */
void bit_decoder_init(bit_decoder_t *bd);

/**
 * Decode FSK symbol to bit(s)
 * NRZI: 1 = no transition, 0 = transition
 * 
 * @param bd Bit decoder instance
 * @param symbol FSK symbol (0 or 1)
 * @return Decoded bit (0 or 1), or -1 if no new bit
 */
int16_t bit_decoder_process(bit_decoder_t *bd, uint8_t symbol);

/**
 * Initialize frame decoder
 */
void frame_decoder_init(frame_decoder_t *fd);

/**
 * Process a decoded bit for frame assembly
 * 
 * @param fd Frame decoder instance
 * @param bit Decoded bit (0 or 1)
 * @return 1 if frame complete, 0 if collecting, -1 on error
 */
int8_t frame_decoder_process(frame_decoder_t *fd, uint8_t bit);

/**
 * Get completed frame
 * 
 * @param fd Frame decoder instance
 * @return Pointer to frame buffer, or NULL if no frame available
 */
uint8_t *frame_decoder_get_frame(frame_decoder_t *fd, uint16_t *length);

/**
 * Parse raw AX.25 frame into fields
 * 
 * @param raw_frame Raw frame bytes (after bit stuffing removal)
 * @param frame_len Frame length in bytes
 * @param frame Parsed frame structure (output)
 * @return 0 on success, -1 on parse error
 */
int8_t ax25_parse_frame(const uint8_t *raw_frame, uint16_t frame_len, ax25_frame_t *frame);

/**
 * Calculate CRC/FCS for frame
 * Uses CRC-CCITT (0x1021)
 * 
 * @param data Frame data
 * @param len Frame length
 * @return Calculated CRC value
 */
uint16_t ax25_calculate_crc(const uint8_t *data, uint16_t len);

/**
 * Check if FCS is valid
 * 
 * @param frame Parsed frame
 * @return 1 if FCS valid, 0 if not
 */
uint8_t ax25_validate_fcs(ax25_frame_t *frame);

#endif /* DECODER_H */
