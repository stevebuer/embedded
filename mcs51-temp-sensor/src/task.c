/*
 * MCS-51 timer routines for 1-wire reader
 *
 * Steve Buer, N7MKO
 * Olympic College
 *
 */

#include <mcs51/8052.h>
#include <stdint.h>
#include "system.h"
#include "systick.h"
#include "cmd.h"
#include "led.h"

/* task data */

typedef struct {

	void (*task)(void);	/* task handler */
	uint16_t period;	/* period in milliseconds */
	uint16_t counter;	/* count */

} task_t;

task_t tasks[] = {

//    { task_temperature, 1000, 0 },
//    { task_button, 500,  0 },
    { task_cmd, 100,  0 },
    { task_led, 1000,  0 }
};

#define NUM_TASKS (sizeof(tasks)/sizeof(task_t))

/* task routines */

void scheduler_run(void) 
{
	uint16_t last_tick = 0;

	if (systick_read() == last_tick)
		return;

	last_tick = systick_read();

	for (unsigned char i = 0; i < NUM_TASKS; i++) {

		if (++tasks[i].counter >= tasks[i].period) {
			
			tasks[i].counter = 0;
			tasks[i].task();
		}
	}
}

void init_task(void)
{
	// todo
}

