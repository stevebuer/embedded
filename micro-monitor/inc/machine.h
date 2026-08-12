/* choose mcu */

#ifdef MCS51

#include <mcs51/8052.h>

extern int putchar(int);

#endif

// HAL

extern void init_system();
extern void init_serial();
extern void init_command();
extern void init_systick();
extern void init_gpio();
extern void init_i2c();
extern void init_onewire();

extern char *readline_serial();
extern char process_command(char *);
