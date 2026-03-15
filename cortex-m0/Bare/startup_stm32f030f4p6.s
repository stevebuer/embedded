.section .text
.global _start

_start:
    b Reset_Handler

Reset_Handler:
    bl main
    b .

    .weak NMI_Handler
    .weak HardFault_Handler
NMI_Handler:
    b .
HardFault_Handler:
    b .
