#ifndef SYSTICK_H
#define SYSTICK_H

extern volatile uint32_t ms_ticks; 

void systick_init(void);
uint32_t get_ticks(void);
// void delay_ms(uint32_t ms);

#endif
