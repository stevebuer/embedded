#include "stm8s_conf.h"
#include "power.h"

void AWU_IRQHandler(void) INTERRUPT(1)
{
}

void power_sleep_30s(void)
{
	AWU_Init(AWU_TIMEBASE_30S);
	AWU_Cmd(ENABLE);

	__asm__("rim");
	__asm__("halt");
	__asm__("sim");

	AWU_Cmd(DISABLE);
}