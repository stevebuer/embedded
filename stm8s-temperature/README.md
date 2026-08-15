# STM8S AHT20 Sensor Monitor

<img src="aht20-monitor.png" width="400">

Outputs raw [AHT20 sensor](AHT20.md) readings to IOT hub for reporting.

## Configuration

EEPROM Config:

* 0x4000: integer number of minutes (0 = off) between automatic sensor reports
* 0x4002: boolean (0 = off) enable power save mode between readings 

## Command Reference

All numeric values for I2C commands are hexadecimal bytes without `0x`.

| Command | Description |
|---|---|
| `h` or `?` | Show help |
| `i` | Scan I2C bus (`0x01`..`0x7E`) |
| `r <addr> <reg>` | Read one byte from I2C register |
| `t` | Read AHT20 |
| `s d` | Set debug mode (no auto report / sleep) |
| `w <addr> <reg> <val>` | Write one byte to I2C register |
| `e <addr> <val>` | Write one byte to STM8 data EEPROM |
| `x <addr>` | Read memory byte |
