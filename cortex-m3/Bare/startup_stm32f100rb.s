.syntax unified
.cpu cortex-m3
.thumb

/* Vector table */
.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
.size g_pfnVectors, .-g_pfnVectors

g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler
    /* External Interrupts */
    .word WWDG_IRQHandler
    .word PVD_IRQHandler
    .word TAMPER_IRQHandler
    .word RTC_IRQHandler
    .word FLASH_IRQHandler
    .word RCC_IRQHandler
    .word EXTI0_IRQHandler
    .word EXTI1_IRQHandler
    .word EXTI2_IRQHandler
    .word EXTI3_IRQHandler
    .word EXTI4_IRQHandler
    .word DMA1_Channel1_IRQHandler
    .word DMA1_Channel2_IRQHandler
    .word DMA1_Channel3_IRQHandler
    .word DMA1_Channel4_IRQHandler
    .word DMA1_Channel5_IRQHandler
    .word DMA1_Channel6_IRQHandler
    .word DMA1_Channel7_IRQHandler
    .word ADC1_2_IRQHandler
    .word USB_HP_CAN1_TX_IRQHandler
    .word USB_LP_CAN1_RX0_IRQHandler
    .word CAN1_RX1_IRQHandler
    .word CAN1_SCE_IRQHandler
    .word EXTI9_5_IRQHandler
    .word TIM1_BRK_IRQHandler
    .word TIM1_UP_IRQHandler
    .word TIM1_TRIG_COM_IRQHandler
    .word TIM1_CC_IRQHandler
    .word TIM2_IRQHandler
    .word TIM3_IRQHandler
    .word TIM4_IRQHandler
    .word I2C1_EV_IRQHandler
    .word I2C1_ER_IRQHandler
    .word I2C2_EV_IRQHandler
    .word I2C2_ER_IRQHandler
    .word SPI1_IRQHandler
    .word SPI2_IRQHandler
    .word USART1_IRQHandler
    .word USART2_IRQHandler
    .word USART3_IRQHandler
    .word EXTI15_10_IRQHandler
    .word RTC_Alarm_IRQHandler
    .word USBWakeUp_IRQHandler

/* Define stack */
_estack = 0x20002000; /* End of 8K RAM */

/* Reset handler */
.section .text
.global Reset_Handler
.type Reset_Handler, %function

Reset_Handler:
/* Copy .data from FLASH to RAM */
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
    movs r3, #0
    b LoopCopyDataInit

CopyDataInit:
    ldr r4, [r2, r3]
    str r4, [r0, r3]
    adds r3, r3, #4

LoopCopyDataInit:
    adds r4, r0, r3
    cmp r4, r1
    bcc CopyDataInit

/* Zero .bss */
    ldr r2, =_sbss
    ldr r4, =_ebss
    movs r3, #0
    b LoopFillZerobss

FillZerobss:
    str r3, [r2]
    adds r2, r2, #4

LoopFillZerobss:
    cmp r2, r4
    bcc FillZerobss

/* Call main */
    bl main
    bx lr

.size Reset_Handler, .-Reset_Handler

/* Default exception handlers */
    .weak NMI_Handler
    .type NMI_Handler, %function
NMI_Handler:
    b .

    .weak HardFault_Handler
    .type HardFault_Handler, %function
HardFault_Handler:
    b .

    .weak MemManage_Handler
    .type MemManage_Handler, %function
MemManage_Handler:
    b .

    .weak BusFault_Handler
    .type BusFault_Handler, %function
BusFault_Handler:
    b .

    .weak UsageFault_Handler
    .type UsageFault_Handler, %function
UsageFault_Handler:
    b .

    .weak SVC_Handler
    .type SVC_Handler, %function
SVC_Handler:
    b .

    .weak DebugMon_Handler
    .type DebugMon_Handler, %function
DebugMon_Handler:
    b .

    .weak PendSV_Handler
    .type PendSV_Handler, %function
PendSV_Handler:
    b .

    .weak SysTick_Handler
    .type SysTick_Handler, %function
SysTick_Handler:
    b .

/* External interrupt handlers (weak) */
    .weak WWDG_IRQHandler
    .type WWDG_IRQHandler, %function
WWDG_IRQHandler:
    b .

    .weak PVD_IRQHandler
    .type PVD_IRQHandler, %function
PVD_IRQHandler:
    b .

    .weak TAMPER_IRQHandler
    .type TAMPER_IRQHandler, %function
TAMPER_IRQHandler:
    b .

    .weak RTC_IRQHandler
    .type RTC_IRQHandler, %function
RTC_IRQHandler:
    b .

    .weak FLASH_IRQHandler
    .type FLASH_IRQHandler, %function
FLASH_IRQHandler:
    b .

    .weak RCC_IRQHandler
    .type RCC_IRQHandler, %function
RCC_IRQHandler:
    b .

    .weak EXTI0_IRQHandler
    .type EXTI0_IRQHandler, %function
EXTI0_IRQHandler:
    b .

    .weak EXTI1_IRQHandler
    .type EXTI1_IRQHandler, %function
EXTI1_IRQHandler:
    b .

    .weak EXTI2_IRQHandler
    .type EXTI2_IRQHandler, %function
EXTI2_IRQHandler:
    b .

    .weak EXTI3_IRQHandler
    .type EXTI3_IRQHandler, %function
EXTI3_IRQHandler:
    b .

    .weak EXTI4_IRQHandler
    .type EXTI4_IRQHandler, %function
EXTI4_IRQHandler:
    b .

    .weak DMA1_Channel1_IRQHandler
    .type DMA1_Channel1_IRQHandler, %function
DMA1_Channel1_IRQHandler:
    b .

    .weak DMA1_Channel2_IRQHandler
    .type DMA1_Channel2_IRQHandler, %function
DMA1_Channel2_IRQHandler:
    b .

    .weak DMA1_Channel3_IRQHandler
    .type DMA1_Channel3_IRQHandler, %function
DMA1_Channel3_IRQHandler:
    b .

    .weak DMA1_Channel4_IRQHandler
    .type DMA1_Channel4_IRQHandler, %function
DMA1_Channel4_IRQHandler:
    b .

    .weak DMA1_Channel5_IRQHandler
    .type DMA1_Channel5_IRQHandler, %function
DMA1_Channel5_IRQHandler:
    b .

    .weak DMA1_Channel6_IRQHandler
    .type DMA1_Channel6_IRQHandler, %function
DMA1_Channel6_IRQHandler:
    b .

    .weak DMA1_Channel7_IRQHandler
    .type DMA1_Channel7_IRQHandler, %function
DMA1_Channel7_IRQHandler:
    b .

    .weak ADC1_2_IRQHandler
    .type ADC1_2_IRQHandler, %function
ADC1_2_IRQHandler:
    b .

    .weak USB_HP_CAN1_TX_IRQHandler
    .type USB_HP_CAN1_TX_IRQHandler, %function
USB_HP_CAN1_TX_IRQHandler:
    b .

    .weak USB_LP_CAN1_RX0_IRQHandler
    .type USB_LP_CAN1_RX0_IRQHandler, %function
USB_LP_CAN1_RX0_IRQHandler:
    b .

    .weak CAN1_RX1_IRQHandler
    .type CAN1_RX1_IRQHandler, %function
CAN1_RX1_IRQHandler:
    b .

    .weak CAN1_SCE_IRQHandler
    .type CAN1_SCE_IRQHandler, %function
CAN1_SCE_IRQHandler:
    b .

    .weak EXTI9_5_IRQHandler
    .type EXTI9_5_IRQHandler, %function
EXTI9_5_IRQHandler:
    b .

    .weak TIM1_BRK_IRQHandler
    .type TIM1_BRK_IRQHandler, %function
TIM1_BRK_IRQHandler:
    b .

    .weak TIM1_UP_IRQHandler
    .type TIM1_UP_IRQHandler, %function
TIM1_UP_IRQHandler:
    b .

    .weak TIM1_TRIG_COM_IRQHandler
    .type TIM1_TRIG_COM_IRQHandler, %function
TIM1_TRIG_COM_IRQHandler:
    b .

    .weak TIM1_CC_IRQHandler
    .type TIM1_CC_IRQHandler, %function
TIM1_CC_IRQHandler:
    b .

    .weak TIM2_IRQHandler
    .type TIM2_IRQHandler, %function
TIM2_IRQHandler:
    b .

    .weak TIM3_IRQHandler
    .type TIM3_IRQHandler, %function
TIM3_IRQHandler:
    b .

    .weak TIM4_IRQHandler
    .type TIM4_IRQHandler, %function
TIM4_IRQHandler:
    b .

    .weak I2C1_EV_IRQHandler
    .type I2C1_EV_IRQHandler, %function
I2C1_EV_IRQHandler:
    b .

    .weak I2C1_ER_IRQHandler
    .type I2C1_ER_IRQHandler, %function
I2C1_ER_IRQHandler:
    b .

    .weak I2C2_EV_IRQHandler
    .type I2C2_EV_IRQHandler, %function
I2C2_EV_IRQHandler:
    b .

    .weak I2C2_ER_IRQHandler
    .type I2C2_ER_IRQHandler, %function
I2C2_ER_IRQHandler:
    b .

    .weak SPI1_IRQHandler
    .type SPI1_IRQHandler, %function
SPI1_IRQHandler:
    b .

    .weak SPI2_IRQHandler
    .type SPI2_IRQHandler, %function
SPI2_IRQHandler:
    b .

    .weak USART1_IRQHandler
    .type USART1_IRQHandler, %function
USART1_IRQHandler:
    b .

    .weak USART2_IRQHandler
    .type USART2_IRQHandler, %function
USART2_IRQHandler:
    b .

    .weak USART3_IRQHandler
    .type USART3_IRQHandler, %function
USART3_IRQHandler:
    b .

    .weak EXTI15_10_IRQHandler
    .type EXTI15_10_IRQHandler, %function
EXTI15_10_IRQHandler:
    b .

    .weak RTC_Alarm_IRQHandler
    .type RTC_Alarm_IRQHandler, %function
RTC_Alarm_IRQHandler:
    b .

    .weak USBWakeUp_IRQHandler
    .type USBWakeUp_IRQHandler, %function
USBWakeUp_IRQHandler:
    b .
