#include "tnc2.h"
#include "serial.h"
#include <string.h>
#include <ctype.h>

/**
 * AX.25 callsigns are encoded with shifted bits
 * Each character is shifted left by 1 bit in the 7-byte address field
 */

static char decode_call_char(uint8_t byte)
{
    /* Shift right by 1 to get original character */
    char c = (byte >> 1) & 0x7F;
    
    /* Valid callsign characters are A-Z, 0-9, space */
    if (c >= 'A' && c <= 'Z')
        return c;
    if (c >= '0' && c <= '9')
        return c;
    if (c == ' ')
        return '\0';  /* Trailing space indicates end */
    
    return '\0';  /* Invalid character */
}

/**
 * Format callsign from 7-byte encoded address
 * Callsign is 6 characters + SSID in 7th byte
 */

uint16_t tnc2_format_callsign(const uint8_t *call, uint8_t ssid, char *buf, uint16_t len)
{
    if (!call || !buf || len < 10)
        return 0;
    
    char *p = buf;
    uint16_t written = 0;
    
    /* Decode 6-character callsign */
    for (int i = 0; i < 6 && written < len - 3; i++)
    {
        char c = decode_call_char(call[i]);
        if (c == '\0')
            break;  /* End of callsign */
        
        *p++ = c;
        written++;
    }
    
    /* Extract SSID from 7th byte (bits 1-4) */
    uint8_t ssid_num = (ssid >> 1) & 0x0F;
    
    /* Add SSID if non-zero */
    if (ssid_num > 0 && written < len - 4)
    {
        *p++ = '-';
        written++;
        
        if (ssid_num < 10)
        {
            *p++ = '0' + ssid_num;
            written++;
        }
        else
        {
            *p++ = '1';
            *p++ = '0' + (ssid_num - 10);
            written += 2;
        }
    }
    
    *p = '\0';
    return written;
}

/**
 * Format complete frame as TNC2
 * SOURCE>DEST[,PATH]:PAYLOAD
 */

int16_t tnc2_format_frame(const ax25_frame_t *frame, char *buf, uint16_t len)
{
    if (!frame || !buf || len < 32)
        return -1;
    
    char *p = buf;
    uint16_t remaining = len;
    uint16_t written = 0;
    
    /* Source callsign */
    uint16_t src_len = tnc2_format_callsign(frame->src_call, frame->src_ssid, p, remaining);
    if (src_len == 0)
        return -1;
    p += src_len;
    remaining -= src_len;
    written += src_len;
    
    /* Separator */
    if (remaining < 2)
        return -1;
    *p++ = '>';
    remaining--;
    written++;
    
    /* Destination callsign */
    uint16_t dst_len = tnc2_format_callsign(frame->dest_call, frame->dest_ssid, p, remaining);
    if (dst_len == 0)
        return -1;
    p += dst_len;
    remaining -= dst_len;
    written += dst_len;
    
    /* Payload (info) separator */
    if (remaining < 2)
        return -1;
    *p++ = ':';
    remaining--;
    written++;
    
    /* Payload data */
    if (frame->payload && frame->payload_len > 0)
    {
        uint16_t copy_len = (frame->payload_len < remaining - 1) ? frame->payload_len : remaining - 1;
        memcpy(p, frame->payload, copy_len);
        p += copy_len;
        remaining -= copy_len;
        written += copy_len;
    }
    
    *p = '\0';
    return written;
}

/**
 * Print frame as TNC2 to serial
 */

void tnc2_print_frame(const ax25_frame_t *frame)
{
    if (!frame)
        return;
    
    char tnc2_buf[128];  /* Reduced from 256 */
    int16_t len = tnc2_format_frame(frame, tnc2_buf, sizeof(tnc2_buf));
    
    if (len > 0)
    {
        serial_putstring(tnc2_buf);
        serial_putstring("\n");
    }
}
