/*
 * System include
 */

// fix these. Separate headers?

uint16_t millis(void);

void init_systick(void);
void init_uart(void);
void init_timer(void);
void init_onewire(void);
void init_task(void);

uint16_t systick_read();

void button_update(void);

uint16_t ds18b20_read_temp(void);
