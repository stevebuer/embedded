# STM8 Humidity Sensor Project

Measure temperature and humidity via i2c sensor.

Board: STM8S-DISCOVERY

MCU: STM8S105 (16 Mhz, 32 KB Flash, 2 KB RAM, 1 KB EEPROM)

## Peripherals

* Touch Sensor
* User LED (Green): Port D, Pin 0

## Toolchain

SDCC and [Ported SPL](https://github.com/bschwand/STM8-SPL-SDCC)

## Files

* stm8s_conf.h    Library Configuration file
* stm8s_it.c:     Source file containing the interrupt handlers (the function bodies are empty in this template).
* stm8s_it.h      Header file including all interrupt handlers prototypes.
* main.c          Main program
* command.c       Serial command processor

## Notes

The STM8105 only has UART2

## References

* [STM8S-DISCOVERY Evaluation Board](https://www.st.com/en/evaluation-tools/stm8s-discovery.html)
* [STM8S105 Data Sheet](https://www.st.com/resource/en/datasheet/stm8s105c4.pdf)
* [STM8 Programming Manual](https://www.st.com/resource/en/programming_manual/pm0044-stm8-cpu-programming-manual-stmicroelectronics.pdf)
