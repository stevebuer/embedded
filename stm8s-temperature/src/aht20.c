/**
 * @file    aht20.c
 * @brief   cross-platform AHT20 driver implementation
 */

#include <stddef.h>
#include "aht20.h"

/* ---- AHT20 commands ---------------------------------- */
#define AHT20_CMD_INIT        0xBEu
#define AHT20_CMD_TRIGGER     0xACu
#define AHT20_CMD_SOFT_RESET  0xBAu

/* Status byte bits */
#define AHT20_STATUS_BUSY_BIT       0x80u
#define AHT20_STATUS_CALIBRATED_BIT 0x08u

/* Timing (datasheet minimums, with a little headroom) */
#define AHT20_POWERUP_DELAY_MS   40u
#define AHT20_INIT_DELAY_MS      10u
#define AHT20_MEASURE_DELAY_MS   80u
#define AHT20_SOFT_RESET_DELAY_MS 20u
#define AHT20_POLL_INTERVAL_MS    5u
#define AHT20_POLL_MAX_ATTEMPTS  20u /* ~100ms of polling before giving up */

static uint8_t aht20_crc8(const uint8_t *data, uint16_t len)
{
	uint8_t crc = 0xFFu;

	for (uint16_t i = 0; i < len; i++) {

		crc ^= data[i];

		for (uint8_t bit = 0; bit < 8; bit++) {
            if (crc & 0x80u) {
                crc = (uint8_t)((crc << 1) ^ 0x31u);
            } else {
                crc = (uint8_t)(crc << 1);
            }
        }
    }
    return crc;
}

static aht20_status_t aht20_write(aht20_ops_t *ops, const uint8_t *data, uint16_t len)
{
	if (ops->i2c_write(ops->ctx, AHT20_I2C_ADDR, data, len) != 0)
		return AHT20_ERR_IO;

	return AHT20_OK;
}

static aht20_status_t aht20_read(aht20_ops_t *ops, uint8_t *data, uint16_t len)
{
	if (ops->i2c_read(ops->ctx, AHT20_I2C_ADDR, data, len) != 0)
		return AHT20_ERR_IO;

	return AHT20_OK;
}

static void aht20_delay(aht20_ops_t *ops, uint32_t ms)
{
	ops->i2c_delay(ms);
}

/** 
 * Poll the status byte until the busy bit clears, or time out. 
 */

static aht20_status_t aht20_wait_ready(aht20_ops_t *ops)
{
	for (uint8_t attempt = 0; attempt < AHT20_POLL_MAX_ATTEMPTS; attempt++) {
		
		uint8_t status = 0;

		aht20_status_t rc = aht20_read(ops, &status, 1);

		if (rc != AHT20_OK)
			return rc;
        
		if ((status & AHT20_STATUS_BUSY_BIT) == 0u)
			return AHT20_OK;
        
		aht20_delay(ops, AHT20_POLL_INTERVAL_MS);
	}

	return AHT20_ERR_TIMEOUT;
}

aht20_status_t aht20_init(aht20_ops_t *ops)
{
	if (ops == NULL)
		return AHT20_ERR_PARAM;
    
	if (ops->i2c_write == NULL || ops->i2c_read == NULL || ops->i2c_delay == NULL)
		return AHT20_ERR_PARAM;

	// init context: To Do:  ops->ctx = *context;

	ops->initialized = false;

	/* Give the sensor time to power up if this runs right after board reset. */

	aht20_delay(ops, AHT20_POWERUP_DELAY_MS);

	uint8_t init_cmd[3] = { AHT20_CMD_INIT, 0x08u, 0x00u };

	aht20_status_t rc = aht20_write(ops, init_cmd, sizeof(init_cmd));

	if (rc != AHT20_OK)
		return rc;
    
	aht20_delay(ops, AHT20_INIT_DELAY_MS);

	bool calibrated = false;

	rc = aht20_is_calibrated(ops, &calibrated);

	if (rc != AHT20_OK)
		return rc;
    
	if (!calibrated)
		return AHT20_ERR_NOT_CALIBRATED;

	ops->initialized = true;

	return AHT20_OK;
}

aht20_status_t aht20_soft_reset(aht20_ops_t *ops)
{
	if (ops == NULL)
		return AHT20_ERR_PARAM;

	uint8_t cmd = AHT20_CMD_SOFT_RESET;

	aht20_status_t rc = aht20_write(ops, &cmd, 1);

	if (rc != AHT20_OK)
		return rc;
  
	ops->initialized = false;

	aht20_delay(ops, AHT20_SOFT_RESET_DELAY_MS);

	return AHT20_OK;
}


aht20_status_t aht20_is_calibrated(aht20_ops_t *ops, bool *calibrated)
{
	if (ops == NULL || calibrated == NULL) 
		return AHT20_ERR_PARAM;

	uint8_t status = 0;

	aht20_status_t rc = aht20_read(ops, &status, 1);

	if (rc != AHT20_OK) 
		return rc;
    
	*calibrated = (status & AHT20_STATUS_CALIBRATED_BIT) != 0u;

	return AHT20_OK;
}

aht20_status_t aht20_read_raw(aht20_ops_t *ops, uint32_t *raw_humidity, uint32_t *raw_temperature)
{
	if (ops == NULL)
		return AHT20_ERR_PARAM;

	if (!ops->initialized)
		return AHT20_ERR_PARAM;

	uint8_t trigger_cmd[3] = { AHT20_CMD_TRIGGER, 0x33u, 0x00u };

	aht20_status_t rc = aht20_write(ops, trigger_cmd, sizeof(trigger_cmd));

	if (rc != AHT20_OK)
		return rc;

	aht20_delay(ops, AHT20_MEASURE_DELAY_MS);

	rc = aht20_wait_ready(ops);

	if (rc != AHT20_OK)
		return rc;

	// status(1) + humidity(2.5 bytes) + temperature(2.5 bytes) + crc(1) = 7 bytes

	uint8_t buf[7] = { 0 };

	rc = aht20_read(ops, buf, sizeof(buf));

	if (rc != AHT20_OK)
		return rc;

	uint8_t crc_calc = aht20_crc8(buf, 6);

	if (crc_calc != buf[6])
		return AHT20_ERR_CRC;

	uint32_t hum_raw  = ((uint32_t) buf[1] << 12) | ((uint32_t) buf[2] << 4) | ((uint32_t) buf[3] >> 4);
	
	uint32_t temp_raw = (((uint32_t) buf[3] & 0x0Fu) << 16) | ((uint32_t) buf[4] << 8) | (uint32_t) buf[5];

	if (raw_humidity != NULL)
		*raw_humidity = hum_raw;
 
	if (raw_temperature != NULL)
		*raw_temperature = temp_raw;
    
	return AHT20_OK;
}

