/*
 * Linker script for STM32F030F4P6
 * 16 KB Flash (0x08000000 - 0x08003FFF)
 * 4 KB SRAM  (0x20000000 - 0x20000FFF)
 */

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 16K
  RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 4K
}

/* Define output sections */
SECTIONS
{
  /* Vector table in flash at start */
  .vectors :
  {
    KEEP(*(.vectors))
  } > FLASH

  /* Code and read-only data in flash */
  .text :
  {
    *(.text*)
    *(.rodata*)
    *(.init*)
    *(.fini*)
    *(.eh_frame*)
  } > FLASH

  /* Initialized data in flash (will be copied to RAM) */
  .data :
  {
    __data_start = .;
    *(.data*)
    __data_end = .;
  } > RAM AT > FLASH

  /* Uninitialized data (BSS) in RAM */
  .bss :
  {
    __bss_start = .;
    *(.bss*)
    *(COMMON)
    __bss_end = .;
  } > RAM

  /* Stack at end of RAM */
  .stack :
  {
    . = ALIGN(8);
    . += 0x200;  /* 512 bytes stack (reduced from 1KB) */
    __StackTop = .;
  } > RAM

  /* Discard debug info in release builds (optional) */
  /DISCARD/ :
  {
    *(.debug*)
  }
}

/* Linker symbols for data/BSS copying */
__etext = LOADADDR(.data);
__data_start = ADDR(.data);
__data_end = ADDR(.data) + SIZEOF(.data);
__bss_start = ADDR(.bss);
__bss_end = ADDR(.bss) + SIZEOF(.bss);
