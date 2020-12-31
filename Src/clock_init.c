#include "clock_init.h"

/**
 * Initializes the clock in the STM32F4RE
 */
void clock_init(void)
{
	// Enables HSE and waits for it to become ready
	*RCC_CR |= (_BV(RCC_CR_HSEON));
	while (!(*RCC_CR & _BV(RCC_CR_HSERDY)));

	// Configures the FLASH
	*RCC_ACR = (_BV(FLASH_ACR_DCEN) | _BV(FLASH_ACR_ICEN) | _BV(FLASH_ACR_PRFTEN) |
			FLASH_ACR_LATENCY(5));

	// Configures HCLK, PCLK1, PCLK2
	*RCC_CFGR &= ~(RCC_CFGR_PPRE2(RCC_CFGR_PPRE_RESET) | RCC_CFGR_PPRE1(RCC_CFGR_PPRE_RESET) |
			RCC_CFGR_HPRE(RCC_CFGR_HPRE_RESET));

	*RCC_CFGR |= (RCC_CFGR_PPRE1(RCC_CFGR_PPRE_DIV4) | RCC_CFGR_PPRE2(RCC_CFGR_PPRE_DIV2) |
			RCC_CFGR_HPRE(RCC_CFGR_HPRE_NDIV));

	// Configures the main PLL
	*RCC_PLLCFGR = (
			RCC_PLLCFGR_PLLN(180) |
			RCC_PLLCFGR_PLLP(RCC_PLLCFGR_PLLP_DIV2) |
			RCC_PLLCFGR_PLLR(2) |
			RCC_PLLCFGR_PLLM(4) |
			_BV(RCC_PLLCFGR_PLLSRC)
	);

	// Enables PLL and waits for it to become ready
	*RCC_CR |= (_BV(RCC_CR_PLLON));
	while (!(*RCC_CR & _BV(RCC_CR_PLLRDY)));

	// Uses PLL as the main clock clock source
	*RCC_CFGR &= ~(RCC_CFGR_SW(RCC_CFGR_SW_RESET));
	*RCC_CFGR |= (RCC_CFGR_SW(RCC_CFGR_SW_PLL_P));
	while ((*RCC_CFGR & RCC_CFGR_SW_MASK) != RCC_CFGR_SW(RCC_CFGR_SW_PLL_P));
}
