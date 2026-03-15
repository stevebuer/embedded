# Steve's Bare Metal Minimum Template

## STM32F030F4P6 Register and Port Configuration Notes

- **RCC_AHBENR (0x40021014):**
  - This register enables clocks for peripherals. To use GPIOA, set bit 17 (GPIOAEN).
  - Example: `RCC_AHBENR |= (1 << 17);` enables GPIOA.

- **GPIOA_MODER (0x48000000):**
  - Controls the mode of each GPIO pin (input, output, alternate, analog).
  - Each pin uses 2 bits. For PA4, clear then set bits 8-9 to `01` for output mode.
  - Example:
    - `GPIOA_MODER &= ~(3 << (4 * 2));` // Clear PA4 mode bits
    - `GPIOA_MODER |=  (1 << (4 * 2));` // Set PA4 to output

- **GPIOA_ODR (0x48000014):**
  - Output Data Register. Controls output state of pins.
  - Example: `GPIOA_ODR ^= (1 << 4);` toggles PA4.

### ARM Cortex-M0 Programming Tips
- Registers are accessed via memory-mapped addresses using volatile pointers.
- Always enable the peripheral clock before configuring GPIO.
- Use bitwise operations to set/clear specific bits.
- Refer to the STM32F030 reference manual for detailed register info.

---

## STM32F030F4P6 Memory Map Overview

- **Flash:** 0x08000000 - 0x08003FFF (16 KB)
- **SRAM:** 0x20000000 - 0x20000FFF (4 KB)
- **Peripheral:** 0x40000000 - 0x500607FF
  - Example: RCC registers at 0x40021000, GPIOA at 0x48000000
- **System Control Space:** 0xE000E000 - 0xE00FFFFF
  - Includes NVIC, SysTick, etc.

### Typical Regions
| Region         | Start       | End         | Size   |
|---------------|------------|-------------|--------|
| Flash         | 0x08000000  | 0x08003FFF  | 16 KB  |
| SRAM          | 0x20000000  | 0x20000FFF  | 4 KB   |
| Peripheral    | 0x40000000  | 0x500607FF  | ~256 KB|
| System Ctrl   | 0xE000E000  | 0xE00FFFFF  | -      |

---

For more details, see the STM32F030 reference manual (RM0360).

If you want more detailed explanations or diagrams, let me know!


