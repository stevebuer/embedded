# Bare Metal LED Cycling for STM32VLDiscovery (STM32F100RB)

This is a bare metal example that polls the user button and cycles through the four onboard LEDs, blinking the current one.

## Hardware

- Board: STM32VLDiscovery
- MCU: STM32F100RB (Cortex-M3)
- LEDs: PD0 (Green), PD1 (Red), PD2 (Orange), PD3 (Blue)
- Button: PA0 (User button B1)

## Prerequisites

- ARM GCC toolchain
- ST-Link tools (st-flash, st-util)
- VS Code with Cortex-Debug extension

## Building

Run `make` to build the binary with debug symbols.

## Flashing

Run `make install` to flash using st-flash.

## Debugging

1. Install the Cortex-Debug extension in VS Code.
2. Plug in your STM32VLDiscovery board.
3. Open this folder in VS Code.
4. Go to Run and Debug (Ctrl+Shift+D), select "Debug STM32F100RB".
5. Click the green play button to start debugging.

The debugger will connect via ST-Link and stop at main().

## Files

- `main.c`: Main blinky code
- `startup_stm32f100rb.s`: Minimal startup assembly
- `stm32f100rb.ld`: Linker script
- `Makefile`: Build script
- `.vscode/launch.json`: Debug configuration