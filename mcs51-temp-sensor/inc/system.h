/*
 * System include
 */

extern volatile uint16_t systick_ms;
extern volatile __bit cmd_flag;

void init_systick(void);
uint16_t millis(void);
uint16_t read_systick(void);

void delay_ms(uint16_t); 

void init_uart(void);
void init_timer(void);
void init_onewire(void);
void init_task(void);

uint16_t systick_read(void);

void button_update(void);

void cmd_process(void);

uint16_t ds18b20_read_temp(void);

/* Interrupt vectors must be declared/included in main.c for proper linking */

void timer0_ISR(void) __interrupt(1);
void serial_ISR(void) __interrupt(4);
