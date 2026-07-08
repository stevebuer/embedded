#include "decoder.h"
#include <string.h>

/* CRC-CCITT polynomial 0x1021 */
#define CRC_POLY 0x1021
#define CRC_INIT 0xFFFF
#define CRC_FINAL_XOR 0xFFFF

/**
 * Initialize bit decoder
 */
void BitDecoder_Init(bit_decoder_t *bd)
{
    bd->last_symbol = 0;
    bd->last_bit = 0;
    bd->bit_buffer = 0;
    bd->bit_count = 0;
    bd->bits_decoded = 0;
    bd->bit_errors = 0;
}

/**
 * NRZI Decoding: 1 = no transition, 0 = transition
 */
int16_t BitDecoder_Process(bit_decoder_t *bd, uint8_t symbol)
{
    /* NRZI decode: compare with last symbol */
    uint8_t bit = (symbol == bd->last_symbol) ? 1 : 0;
    bd->last_symbol = symbol;
    
    return (int16_t)bit;
}

/**
 * Initialize frame decoder
 */
void FrameDecoder_Init(frame_decoder_t *fd)
{
    fd->state = STATE_SYNCING;
    fd->frame_length = 0;
    fd->frame_count = 0;
    fd->bit_buffer = 0;
    fd->bit_count = 0;
    fd->stuff_count = 0;
    fd->frame_flags_seen = 0;
    fd->fcs = CRC_INIT;
    fd->fcs_valid = 0;
    fd->bits_since_flag = 0;
    fd->frames_received = 0;
    fd->frames_with_bad_fcs = 0;
    fd->sync_losses = 0;
}

/**
 * Update CRC/FCS with new bit
 */
static void UpdateFCS(uint16_t *fcs, uint8_t bit)
{
    uint16_t xor_in = (*fcs ^ ((uint16_t)bit << 15)) & 0x8000;
    *fcs <<= 1;
    if (xor_in)
        *fcs ^= CRC_POLY;
    *fcs &= 0xFFFF;
}

/**
 * Process bit for frame assembly
 * Handles:
 * - Frame sync (0x7E flag detection)
 * - Bit stuffing removal (5 consecutive 1s -> skip next 0)
 * - FCS calculation
 */
int8_t FrameDecoder_Process(frame_decoder_t *fd, uint8_t bit)
{
    fd->bits_since_flag++;
    
    switch (fd->state)
    {
        case STATE_SYNCING:
            /* Look for 0x7E flag byte (01111110) */
            fd->bit_buffer = (fd->bit_buffer << 1) | (bit & 1);
            fd->bit_count++;
            
            if (fd->bit_count == 8)
            {
                if (fd->bit_buffer == AX25_FLAG)
                {
                    fd->frame_flags_seen++;
                    
                    /* Start of frame on second flag */
                    if (fd->frame_flags_seen >= 2)
                    {
                        fd->state = STATE_IN_FRAME;
                        fd->frame_length = 0;
                        fd->fcs = CRC_INIT;
                        fd->stuff_count = 0;
                        fd->bits_since_flag = 0;
                    }
                }
                else
                {
                    fd->frame_flags_seen = 0;  /* Reset if not a flag */
                }
                
                fd->bit_buffer = 0;
                fd->bit_count = 0;
            }
            break;
            
        case STATE_IN_FRAME:
            /* Check for bit stuffing (5 consecutive 1s) */
            if (bit)
            {
                fd->stuff_count++;
            }
            else
            {
                /* After 5 ones, next 0 is stuffed - skip it */
                if (fd->stuff_count == 5)
                {
                    fd->stuff_count = 0;
                    break;  /* Skip stuffed bit */
                }
                fd->stuff_count = 0;
            }
            
            /* Accumulate bits into byte */
            fd->bit_buffer = (fd->bit_buffer << 1) | (bit & 1);
            fd->bit_count++;
            
            /* When byte is complete */
            if (fd->bit_count == 8)
            {
                /* Update FCS before adding to frame */
                for (int i = 0; i < 8; i++)
                {
                    UpdateFCS(&fd->fcs, (fd->bit_buffer >> (7 - i)) & 1);
                }
                
                /* Check for frame end flag (0x7E) */
                if (fd->bit_buffer == AX25_FLAG)
                {
                    /* End of frame */
                    fd->state = STATE_FRAME_COMPLETE;
                    fd->frames_received++;
                    fd->bit_count = 0;
                    fd->bit_buffer = 0;
                    fd->bits_since_flag = 0;
                    
                    /* Frame is complete, length is ready */
                    return 1;
                }
                else
                {
                    /* Add byte to frame */
                    if (fd->frame_length < AX25_MAX_FRAME_LEN)
                    {
                        fd->frame_buffer[fd->frame_length++] = fd->bit_buffer;
                    }
                    else
                    {
                        /* Frame too long */
                        fd->state = STATE_SYNCING;
                        fd->frame_flags_seen = 0;
                        fd->sync_losses++;
                        return -1;
                    }
                }
                
                fd->bit_count = 0;
                fd->bit_buffer = 0;
            }
            break;
            
        case STATE_FRAME_COMPLETE:
            /* Return to syncing state */
            fd->state = STATE_SYNCING;
            fd->frame_flags_seen = 0;
            fd->bit_count = 0;
            fd->bit_buffer = 0;
            break;
            
        case STATE_ERROR:
            fd->state = STATE_SYNCING;
            fd->frame_flags_seen = 0;
            break;
    }
    
    return 0;  /* No complete frame yet */
}

/**
 * Get completed frame
 */
uint8_t *FrameDecoder_GetFrame(frame_decoder_t *fd, uint16_t *length)
{
    if (fd->state == STATE_FRAME_COMPLETE && fd->frame_length > 0)
    {
        *length = fd->frame_length;
        fd->state = STATE_SYNCING;
        fd->frame_length = 0;
        return fd->frame_buffer;
    }
    
    *length = 0;
    return NULL;
}

/**
 * Calculate CRC-CCITT for frame
 */
uint16_t AX25_CalculateCRC(const uint8_t *data, uint16_t len)
{
    uint16_t crc = CRC_INIT;
    
    for (uint16_t i = 0; i < len; i++)
    {
        uint8_t byte = data[i];
        for (int bit = 0; bit < 8; bit++)
        {
            UpdateFCS(&crc, (byte >> bit) & 1);
        }
    }
    
    return crc ^ CRC_FINAL_XOR;
}

/**
 * Parse AX.25 frame
 * Structure: [flag] [dest_addr(7)] [src_addr(7)] [control] [pid] [data...] [fcs(2)] [flag]
 */
int8_t AX25_ParseFrame(const uint8_t *raw_frame, uint16_t frame_len, 
                        ax25_frame_t *frame)
{
    if (frame_len < AX25_MIN_FRAME_LEN)
        return -1;  /* Frame too short */
    
    uint16_t pos = 0;
    
    /* Extract destination address (7 bytes, bits shifted) */
    memcpy(frame->dest_call, &raw_frame[pos], 6);
    frame->dest_ssid = raw_frame[pos + 6];
    pos += 7;
    
    /* Extract source address (7 bytes) */
    memcpy(frame->src_call, &raw_frame[pos], 6);
    frame->src_ssid = raw_frame[pos + 6];
    pos += 7;
    
    /* Extract control byte */
    if (pos >= frame_len)
        return -1;
    frame->control = raw_frame[pos++];
    
    /* Extract PID (if I-frame) */
    if ((frame->control & 0x01) == 0)  /* I-frame */
    {
        if (pos >= frame_len)
            return -1;
        frame->pid = raw_frame[pos++];
    }
    else
    {
        frame->pid = 0;
    }
    
    /* Remaining bytes are payload + FCS */
    if (frame_len - pos < 2)
        return -1;  /* No room for FCS */
    
    /* Extract FCS (last 2 bytes, little-endian) */
    frame->fcs = (raw_frame[frame_len - 1] << 8) | raw_frame[frame_len - 2];
    
    /* Payload is between PID and FCS */
    frame->payload_len = frame_len - pos - 2;
    frame->payload = (uint8_t *)&raw_frame[pos];
    
    /* Calculate and verify FCS */
    frame->fcs_calculated = AX25_CalculateCRC(raw_frame, frame_len - 2);
    frame->fcs_valid = (frame->fcs == frame->fcs_calculated) ? 1 : 0;
    
    return 0;
}

/**
 * Validate FCS
 */
uint8_t AX25_ValidateFCS(ax25_frame_t *frame)
{
    return frame->fcs_valid;
}
