#include "systick.h"

static volatile u64 __systick_overflow_count = 0;

void SysTick_Handler(void)
{
	++__systick_overflow_count;
}

void systick_init(void)
{
	*STK_VAL = STK_LOAD_VALUE;
	*STK_LOAD = STK_LOAD_VALUE;
	*STK_CALIB = STK_CALIB_VALUE;
	*STK_CTRL = (_BV(STK_CTRL_TICKINT)
			| _BV(STK_CTRL_ENABLE));
}

u64 systick_get_time(void)
{
	u64 time = (u64) *STK_VAL;
	time += (__systick_overflow_count * STK_LOAD_VALUE);
	return time / 30;
}
