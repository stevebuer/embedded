#ifndef TNC2_H
#define TNC2_H

#include "decoder.h"

/**
 * TNC2 Format Output
 * 
 * Standard ASCII format for AX.25 frames:
 * SOURCE>DEST[,PATH]:PAYLOAD
 * 
 * Example:
 * N0CALL-1>W5XYZ,W5ABC,W5DEF:Hello World
 */

/**
 * Convert AX.25 callsign (7 bytes with SSID) to ASCII string
 * Format: "CALLSIGN-N" or just "CALLSIGN" if SSID is 0
 * 
 * @param call Raw 7-byte callsign (bits 1-7 of each byte are shifted)
 * @param ssid SSID byte
 * @param buf Output buffer (minimum 10 bytes)
 * @param len Buffer length
 * @return Number of characters written
 */
uint16_t TNC2_FormatCallsign(const uint8_t *call, uint8_t ssid, 
                              char *buf, uint16_t len);

/**
 * Format complete AX.25 frame as TNC2 ASCII
 * 
 * @param frame Parsed AX.25 frame
 * @param buf Output buffer
 * @param len Buffer length
 * @return Number of characters written, or -1 on error
 */
int16_t TNC2_FormatFrame(const ax25_frame_t *frame, char *buf, uint16_t len);

/**
 * Output frame as TNC2 to serial
 * Convenience function that formats and prints in one call
 * 
 * @param frame Parsed AX.25 frame
 */
void TNC2_PrintFrame(const ax25_frame_t *frame);

#endif /* TNC2_H */
