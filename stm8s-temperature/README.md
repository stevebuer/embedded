# STM8 Serial Monitor

Simple command monitor over UART for STM8S003 with I2C utilities and a small shell-like command interface for peripheral control.

## Command Reference

All numeric values for I2C commands are hexadecimal bytes without `0x`.

| Command | Description |
|---|---|
| `h` or `?` | Show help |
| `i` | Scan I2C bus (`0x01`..`0x7E`) |
| `b` | Test BMP180 at `0x77` by verifying chip ID `0x55` |
| `r <addr> <reg>` | Read one byte from I2C register |
| `w <addr> <reg> <val>` | Write one byte to I2C register |
| `e <addr> <val>` | Write one byte to STM8 data EEPROM |
| `ow ...` | Dallas 1-Wire command group |

## I2C Command Notes

- `addr` is the 7-bit I2C address (example: BMP180 is `77`).
- `reg` and `val` are one-byte hex values.
- Read/write commands return:
  - `ERR: i2c no-ack` when no device acknowledges the address.
  - `ERR: i2c timeout` on bus or transaction timeout.

## 1-Wire Commands

- The 1-Wire data bus is on PD2; use an external pull-up resistor (typically 4.7 kOhm) to the sensor supply.
- `ow reset` checks for a device presence pulse.
- `ow scan` and `ow readrom` issue the `READ ROM` command and print the eight-byte ROM code. They require exactly one device on the bus.
- A ROM beginning with `0x28` is identified as a DS18B20-family device.

## Example Session

<pre>
micro-mon: v0.3
  h|?          help
  i            scan i2c bus
  b            test BMP180 chip ID
  r a r        read i2c register (hex bytes)
  w a r v      write i2c register (hex bytes)
  e addr v     write byte to stm8 data EEPROM
  ow ...       dallas 1-wire commands
cmd> i
I2C scan start
  found 0x38
  found 0x77
I2C scan done
cmd> r 77 d0
  0x55
cmd> ow reset
1W: device present
cmd>
</pre>
