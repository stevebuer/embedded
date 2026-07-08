/*
 * STM32F030F4P6 Cortex M0 Startup Code
 * Initializes stack, BSS, and data sections before jumping to main()
 */

    .syntax unified
    .cpu cortex-m0
    .thumb

/*
 * Memory layout for STM32F030F4P6:
 * - Flash: 16 KB (0x08000000 - 0x08003FFF)
 * - SRAM:  4 KB  (0x20000000 - 0x20000FFF)
 */

    .section .stack
    .align 3
    .globl __StackTop
    .globl __StackLimit

__StackLimit:
    .space 0x400  /* 1 KB stack */
__StackTop:

    .section .data
    .align 2

    .section .bss
    .align 2

/*
 * Vector Table - must be at base of flash (0x08000000)
 */
    .section .vectors, "a"
    .align 2
    .globl __Vectors
    .globl __Vectors_End
    .globl __Vectors_Size

__Vectors:
    .long   __StackTop              /* 0x00: Stack pointer */
    .long   Reset_Handler           /* 0x04: Reset */
    .long   NMI_Handler             /* 0x08: NMI */
    .long   HardFault_Handler       /* 0x0C: Hard Fault */
    .long   0                       /* 0x10: Reserved */
    .long   0                       /* 0x14: Reserved */
    .long   0                       /* 0x18: Reserved */
    .long   0                       /* 0x1C: Reserved */
    .long   0                       /* 0x20: Reserved */
    .long   0                       /* 0x24: Reserved */
    .long   0                       /* 0x28: Reserved */
    .long   SVC_Handler             /* 0x2C: SVCall */
    .long   0                       /* 0x30: Reserved */
    .long   0                       /* 0x34: Reserved */
    .long   PendSV_Handler          /* 0x38: PendSV */
    .long   SysTick_Handler         /* 0x3C: SysTick */
    
    /* STM32F030 External Interrupts */
    .long   WWDG_Handler            /* 0x40: Window Watchdog */
    .long   0                       /* 0x44: Reserved */
    .long   RTC_Handler             /* 0x48: RTC */
    .long   FLASH_Handler           /* 0x4C: Flash */
    .long   RCC_Handler             /* 0x50: RCC */
    .long   EXTI0_1_Handler         /* 0x54: EXTI Line 0 and 1 */
    .long   EXTI2_3_Handler         /* 0x58: EXTI Line 2 and 3 */
    .long   EXTI4_15_Handler        /* 0x5C: EXTI Line 4 to 15 */
    .long   0                       /* 0x60: Reserved */
    .long   DMA1_CH1_Handler        /* 0x64: DMA1 Channel 1 */
    .long   DMA1_CH2_3_Handler      /* 0x68: DMA1 Channel 2 and 3 */
    .long   DMA1_CH4_5_Handler      /* 0x6C: DMA1 Channel 4 and 5 */
    .long   ADC1_Handler            /* 0x70: ADC1 */
    .long   TIM1_BRK_Handler        /* 0x74: TIM1 Break */
    .long   TIM1_UP_Handler         /* 0x78: TIM1 Update */
    .long   TIM1_TRIG_COM_Handler   /* 0x7C: TIM1 Trigger and Commutation */
    .long   TIM1_CC_Handler         /* 0x80: TIM1 Capture Compare */
    .long   TIM3_Handler            /* 0x84: TIM3 */
    .long   0                       /* 0x88: Reserved */
    .long   0                       /* 0x8C: Reserved */
    .long   TIM14_Handler           /* 0x90: TIM14 */
    .long   0                       /* 0x94: Reserved */
    .long   TIM16_Handler           /* 0x98: TIM16 */
    .long   TIM17_Handler           /* 0x9C: TIM17 */
    .long   I2C1_Handler            /* 0xA0: I2C1 */
    .long   I2C2_Handler            /* 0xA4: I2C2 */
    .long   SPI1_Handler            /* 0xA8: SPI1 */
    .long   SPI2_Handler            /* 0xAC: SPI2 */
    .long   USART1_Handler          /* 0xB0: USART1 */
    .long   USART2_Handler          /* 0xB4: USART2 */
    .long   0                       /* 0xB8: Reserved */
    .long   CEC_Handler             /* 0xBC: CEC */

__Vectors_End:
    .equ __Vectors_Size, __Vectors_End - __Vectors

/*
 * Reset Handler
 * Initializes memory and calls main()
 */
    .section .text
    .align 2
    .globl Reset_Handler
    .type Reset_Handler, %function

Reset_Handler:
    /* Disable interrupts */
    cpsid i
    
    /* Copy initialized data from flash to SRAM */
    ldr r0, =__etext        /* Source: end of flash code section */
    ldr r1, =__data_start   /* Destination: start of data section */
    ldr r2, =__data_end     /* End of data section */
    
    cmp r0, r1
    beq 2f                  /* Skip if no data to copy */
    
1:  cmp r1, r2
    bge 2f
    ldr r3, [r0]
    str r3, [r1]
    adds r0, r0, #4
    adds r1, r1, #4
    b 1b
    
    /* Zero out BSS section */
2:  ldr r0, =__bss_start    /* Start of BSS */
    ldr r1, =__bss_end      /* End of BSS */
    movs r2, #0
    
    cmp r0, r1
    beq 3f
    
1:  cmp r0, r1
    bge 3f
    str r2, [r0]
    adds r0, r0, #4
    b 1b
    
    /* Call SystemInit() if defined */
3:  bl SystemInit
    
    /* Call main() */
    bl main
    
    /* Infinite loop if main() returns */
    b .

/*
 * Default Exception Handlers
 * Weak symbols so they can be overridden
 */
    .section .text
    .align 1
    
    /* Generic default handler */
    .macro def_handler handler_name
    .weak \handler_name
    .set \handler_name, Default_Handler
    .endm
    
    .globl Default_Handler
    .type Default_Handler, %function
    
Default_Handler:
    b Default_Handler       /* Infinite loop */
    
    /* Define all weak handlers */
    def_handler NMI_Handler
    def_handler HardFault_Handler
    def_handler SVC_Handler
    def_handler PendSV_Handler
    def_handler SysTick_Handler
    def_handler WWDG_Handler
    def_handler RTC_Handler
    def_handler FLASH_Handler
    def_handler RCC_Handler
    def_handler EXTI0_1_Handler
    def_handler EXTI2_3_Handler
    def_handler EXTI4_15_Handler
    def_handler DMA1_CH1_Handler
    def_handler DMA1_CH2_3_Handler
    def_handler DMA1_CH4_5_Handler
    def_handler ADC1_Handler
    def_handler TIM1_BRK_Handler
    def_handler TIM1_UP_Handler
    def_handler TIM1_TRIG_COM_Handler
    def_handler TIM1_CC_Handler
    def_handler TIM3_Handler
    def_handler TIM14_Handler
    def_handler TIM16_Handler
    def_handler TIM17_Handler
    def_handler I2C1_Handler
    def_handler I2C2_Handler
    def_handler SPI1_Handler
    def_handler SPI2_Handler
    def_handler USART1_Handler
    def_handler USART2_Handler
    def_handler CEC_Handler

/*
 * Linker script symbols (referenced by Reset_Handler)
 * These should be defined in your linker script
 */
    .weak __etext
    .weak __data_start
    .weak __data_end
    .weak __bss_start
    .weak __bss_end
    .weak SystemInit

    .end
