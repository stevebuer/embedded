#ifndef I2C_BUS_H
#define I2C_BUS_H

#ifndef __STM8S_H
#include <stdint.h>
#endif

#define I2C_PROBE_NONE    0
#define I2C_PROBE_FOUND   1
#define I2C_PROBE_TIMEOUT 2

void i2c_bus_init(void);
void i2c_bus_scan(void);
unsigned char i2c_bus_read_reg(unsigned char addr, unsigned char reg, unsigned char* value);
unsigned char i2c_bus_write_reg(unsigned char addr, unsigned char reg, unsigned char value);

int i2c_write(void *ctx, uint8_t addr, const uint8_t *data, uint16_t len);
int i2c_read(void *ctx, uint8_t addr, uint8_t *data, uint16_t len);
void i2c_delay(uint32_t ms);

#endif