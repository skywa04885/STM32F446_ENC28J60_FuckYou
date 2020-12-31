#include "delay.h"

void delay_init(void)
{
	// Enables the clock for TIM11
	*RCC_APB2ENR |= (_BV(RCC_APB2ENR_TIM11EN));

	// Configures the TIM11 peripheral
	*TIM_CR1(TIM11_BASE) = (_BV(TIM_CR1_UDIS));
	*TIM_PSC(TIM11_BASE) = 90;			// 90Mhz / 90 = 1Mhz
}

void delay_us(u16 us)
{
	*TIM_CNT(TIM11_BASE) = 0x00;
	*TIM_CR1(TIM11_BASE) |= (_BV(TIM_CR1_CEN));
	while (*TIM_CNT(TIM11_BASE) < us);
	*TIM_CR1(TIM11_BASE) &= (_BV(TIM_CR1_CEN));
}

void delay_ms(u16 ms)
{
	for (u16 i = 0; i < ms; ++i)
	{
		delay_us(1000);
	}
}

void delay_s(u16 s)
{
	for (u16 i = 0; i < s; ++i)
	{
		delay_ms(1000);
	}
}
