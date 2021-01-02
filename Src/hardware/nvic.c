#include "nvic.h"

void nvic_enable_interrupt(u32 n)
{
	if (n >= 0 && n <= 31) *NBIC_ISER_BASE |= (_BV(n));
}
