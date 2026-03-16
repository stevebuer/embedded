# Arm Cortex M0 Development Board

<img src="stm32f030_minimal.jpg">

* stm32f030f4p6: 48 Mhz Clock, 16KB Flash, 4KB RAM, 20 Pin TSSOP

## STM32CubeMX Configuration

* SYS.Debug must be set to SWD because these pins can conflict with gpio and render chip unuseable.

## Programming

* SWD: st-link, openocd

## Examples

* Blinky

## Documentation

* [STM32F030F4P6 data sheet](https://www.st.com/resource/en/datasheet/stm32f030f4.pdf)
