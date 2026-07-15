/*
 * onewire.h
 */

void init_onewire(void);
void ow_write_byte(unsigned char);
void ow_delay_us(unsigned int);
unsigned char ow_reset();
unsigned char ow_read_byte(void);

