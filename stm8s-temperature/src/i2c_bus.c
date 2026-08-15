#include "stm8s_conf.h"
#include "uart.h"
#include "i2c_bus.h"

#define I2C_WAIT_TIMEOUT 50000U

static uint8_t i2c_wait_flag_clear(I2C_Flag_TypeDef flag)
{
	uint16_t t = I2C_WAIT_TIMEOUT;

	while (I2C_GetFlagStatus(flag)) {
		if (--t == 0) {
			return 0;
		}
	}

	return 1;
}

static uint8_t i2c_wait_event(uint16_t event)
{
	uint16_t t = I2C_WAIT_TIMEOUT;

	while (!I2C_CheckEvent(event)) {
		if (--t == 0) {
			return 0;
		}
	}

	return 1;
}

static uint8_t i2c_wait_byte_transmitted(void)
{
	uint16_t t = I2C_WAIT_TIMEOUT;

	while (t--) {
		if (I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			return I2C_PROBE_FOUND;

		if (I2C_GetFlagStatus(I2C_FLAG_ACKNOWLEDGEFAILURE)) {
			I2C_ClearFlag(I2C_FLAG_ACKNOWLEDGEFAILURE);
			return I2C_PROBE_NONE;
		}
	}

	return I2C_PROBE_TIMEOUT;
}

static uint8_t i2c_probe(uint8_t addr)
{
	uint16_t t;

	if (!i2c_wait_flag_clear(I2C_FLAG_BUSBUSY)) {
		I2C_SoftwareResetCmd(ENABLE);
		I2C_SoftwareResetCmd(DISABLE);
		return I2C_PROBE_TIMEOUT;
	}

	I2C_GenerateSTART(ENABLE);
	if (!i2c_wait_event(I2C_EVENT_MASTER_MODE_SELECT)) {
		I2C_GenerateSTOP(ENABLE);
		return I2C_PROBE_TIMEOUT;
	}

	I2C_Send7bitAddress((uint8_t)(addr << 1), I2C_DIRECTION_TX);

	t = I2C_WAIT_TIMEOUT;
	while (t--) {
		if (I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
			I2C_GenerateSTOP(ENABLE);
			return I2C_PROBE_FOUND;
		}

		if (I2C_GetFlagStatus(I2C_FLAG_ACKNOWLEDGEFAILURE)) {
			I2C_ClearFlag(I2C_FLAG_ACKNOWLEDGEFAILURE);
			I2C_GenerateSTOP(ENABLE);
			return I2C_PROBE_NONE;
		}
	}

	I2C_ClearFlag(I2C_FLAG_ACKNOWLEDGEFAILURE);
	I2C_GenerateSTOP(ENABLE);
	return I2C_PROBE_TIMEOUT;
}

void i2c_bus_init(void)
{
	/* STM8S003 I2C pins: PB4=SCL, PB5=SDA (open-drain). */
	GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)(GPIO_PIN_4 | GPIO_PIN_5), GPIO_MODE_OUT_OD_HIZ_FAST);

	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
	I2C_Cmd(DISABLE);
	I2C_DeInit();

	/*
	 * 100 kHz bus, no slave addressing in this app, no ACK handling needed
	 * for address-probe scans.
	 */
	I2C_Init(100000, 0x00, I2C_DUTYCYCLE_2, I2C_ACK_NONE, I2C_ADDMODE_7BIT, 16);
	I2C_Cmd(ENABLE);
}

int i2c_write(void *ctx, uint8_t addr, const uint8_t *data, uint16_t len)
{
	(void)ctx;

	if (!i2c_wait_flag_clear(I2C_FLAG_BUSBUSY))
		return I2C_PROBE_TIMEOUT;

	I2C_GenerateSTART(ENABLE);
	if (!i2c_wait_event(I2C_EVENT_MASTER_MODE_SELECT))
		goto error;

	I2C_Send7bitAddress((uint8_t)(addr << 1), I2C_DIRECTION_TX);
	if (!i2c_wait_event(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		goto error;

	while (len--) {
		I2C_SendData(*data++);
		if (i2c_wait_byte_transmitted() != I2C_PROBE_FOUND)
			goto error;
	}

	I2C_GenerateSTOP(ENABLE);
	return 0;

error:
	I2C_ClearFlag(I2C_FLAG_ACKNOWLEDGEFAILURE);
	I2C_GenerateSTOP(ENABLE);
	return -1;
}

int i2c_read(void *ctx, uint8_t addr, uint8_t *data, uint16_t len)
{
	uint16_t index;

	(void)ctx;

	if (len == 0 || !i2c_wait_flag_clear(I2C_FLAG_BUSBUSY))
		return -1;

	I2C_AcknowledgeConfig(ENABLE);
	I2C_GenerateSTART(ENABLE);
	if (!i2c_wait_event(I2C_EVENT_MASTER_MODE_SELECT))
		goto error;

	I2C_Send7bitAddress((uint8_t)(addr << 1), I2C_DIRECTION_RX);
	if (!i2c_wait_event(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		goto error;

	for (index = 0; index < len; index++) {
		if (index == (uint16_t)(len - 1))
			I2C_AcknowledgeConfig(I2C_ACK_NONE);

		if (!i2c_wait_event(I2C_EVENT_MASTER_BYTE_RECEIVED))
			goto error;

		data[index] = I2C_ReceiveData();
	}

	I2C_GenerateSTOP(ENABLE);
	I2C_AcknowledgeConfig(I2C_ACK_NONE);
	return 0;

error:
	I2C_GenerateSTOP(ENABLE);
	I2C_AcknowledgeConfig(I2C_ACK_NONE);
	return -1;
}

void i2c_delay(uint32_t ms)
{
	/* loop body is 12 cycles/iter at -opt-code-size; 1333 iters ~= 1ms @ 16MHz HSI */
	while (ms--) {
		for (volatile uint16_t count = 0; count < 1333; count++) {
		}
	}
}

unsigned char i2c_bus_write_reg(unsigned char addr, unsigned char reg, unsigned char value)
{
	uint8_t result;

	if (!i2c_wait_flag_clear(I2C_FLAG_BUSBUSY)) {
		I2C_SoftwareResetCmd(ENABLE);
		I2C_SoftwareResetCmd(DISABLE);
		return I2C_PROBE_TIMEOUT;
	}

	I2C_GenerateSTART(ENABLE);
	if (!i2c_wait_event(I2C_EVENT_MASTER_MODE_SELECT)) {
		I2C_GenerateSTOP(ENABLE);
		return I2C_PROBE_TIMEOUT;
	}

	I2C_Send7bitAddress((uint8_t)(addr << 1), I2C_DIRECTION_TX);
	if (!i2c_wait_event(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if (I2C_GetFlagStatus(I2C_FLAG_ACKNOWLEDGEFAILURE)) {
			I2C_ClearFlag(I2C_FLAG_ACKNOWLEDGEFAILURE);
			I2C_GenerateSTOP(ENABLE);
			return I2C_PROBE_NONE;
		}
		I2C_GenerateSTOP(ENABLE);
		return I2C_PROBE_TIMEOUT;
	}

	I2C_SendData(reg);
	result = i2c_wait_byte_transmitted();
	if (result != I2C_PROBE_FOUND) {
		I2C_GenerateSTOP(ENABLE);
		return result;
	}

	I2C_SendData(value);
	result = i2c_wait_byte_transmitted();
	if (result != I2C_PROBE_FOUND) {
		I2C_GenerateSTOP(ENABLE);
		return result;
	}

	I2C_GenerateSTOP(ENABLE);
	return I2C_PROBE_FOUND;
}

unsigned char i2c_bus_read_reg(unsigned char addr, unsigned char reg, unsigned char* value)
{
	uint8_t result;

	if (!i2c_wait_flag_clear(I2C_FLAG_BUSBUSY)) {
		I2C_SoftwareResetCmd(ENABLE);
		I2C_SoftwareResetCmd(DISABLE);
		return I2C_PROBE_TIMEOUT;
	}

	I2C_GenerateSTART(ENABLE);
	if (!i2c_wait_event(I2C_EVENT_MASTER_MODE_SELECT)) {
		I2C_GenerateSTOP(ENABLE);
		return I2C_PROBE_TIMEOUT;
	}

	I2C_Send7bitAddress((uint8_t)(addr << 1), I2C_DIRECTION_TX);
	if (!i2c_wait_event(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
		if (I2C_GetFlagStatus(I2C_FLAG_ACKNOWLEDGEFAILURE)) {
			I2C_ClearFlag(I2C_FLAG_ACKNOWLEDGEFAILURE);
			I2C_GenerateSTOP(ENABLE);
			return I2C_PROBE_NONE;
		}
		I2C_GenerateSTOP(ENABLE);
		return I2C_PROBE_TIMEOUT;
	}

	I2C_SendData(reg);
	result = i2c_wait_byte_transmitted();
	if (result != I2C_PROBE_FOUND) {
		I2C_GenerateSTOP(ENABLE);
		return result;
	}

	I2C_GenerateSTART(ENABLE);
	if (!i2c_wait_event(I2C_EVENT_MASTER_MODE_SELECT)) {
		I2C_GenerateSTOP(ENABLE);
		return I2C_PROBE_TIMEOUT;
	}

	I2C_Send7bitAddress((uint8_t)(addr << 1), I2C_DIRECTION_RX);
	if (!i2c_wait_event(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
		if (I2C_GetFlagStatus(I2C_FLAG_ACKNOWLEDGEFAILURE)) {
			I2C_ClearFlag(I2C_FLAG_ACKNOWLEDGEFAILURE);
			I2C_GenerateSTOP(ENABLE);
			return I2C_PROBE_NONE;
		}
		I2C_GenerateSTOP(ENABLE);
		return I2C_PROBE_TIMEOUT;
	}

	/* For one byte, send NACK and STOP before waiting for RXNE. */
	I2C_AcknowledgeConfig(I2C_ACK_NONE);
	I2C_GenerateSTOP(ENABLE);

	if (!i2c_wait_event(I2C_EVENT_MASTER_BYTE_RECEIVED)) {
		return I2C_PROBE_TIMEOUT;
	}

	*value = I2C_ReceiveData();
	return I2C_PROBE_FOUND;
}

void i2c_bus_scan(void)
{
	uint8_t result;
	uint8_t found = 0;

	uart_puts("I2C scan start\r\n");

	for (uint8_t addr = 1; addr < 127; addr++) {
		result = i2c_probe(addr);

		if (result == I2C_PROBE_FOUND) {
			uart_puts("  found 0x");
			uart_puthex8(addr);
			uart_puts("\r\n");
			found++;
		} else if (result == I2C_PROBE_TIMEOUT) {
			uart_puts("  ERR: i2c timeout (check pullups, wiring, PB4/PB5 usage)\r\n");
			break;
		}
	}

	if (found == 0) {
		uart_puts("  no devices\r\n");
	}

	uart_puts("I2C scan done\r\n\r\n");
}