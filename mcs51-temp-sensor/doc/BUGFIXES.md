1. Systick not working: Timer0 ISR not being linked into final program. Disassembly of vector table showed missing entry.
   Fix: ISR must be declared in main.c to be linked.

2. Once Timer fixed, no serial output: Serial ISR is clearing TI -- wrong! Putchar handles TI.
