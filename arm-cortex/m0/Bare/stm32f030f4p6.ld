/* Minimal linker script for STM32F030F4P6 */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 16K
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 4K
}

SECTIONS
{
  .text :
  {
    *(.text)
    *(.rodata)
    *(.data)
    *(.bss)
  } > FLASH

  .data :
  {
    *(.data)
  } > RAM AT > FLASH

  .bss :
  {
    *(.bss)
  } > RAM
}
