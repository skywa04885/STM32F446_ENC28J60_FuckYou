#ifndef _SRC_HARDWARE_RCC_H
#define _SRC_HARDWARE_RCC_H

#include "../types.h"

#define RCC_BASE						0x40023800

#define RCC_CR							((u32 *) (RCC_BASE + 0x00))
#define RCC_PLLCFGR						((u32 *) (RCC_BASE + 0x04))
#define RCC_CFGR						((u32 *) (RCC_BASE + 0x08))
#define RCC_AHB1ENR						((u32 *) (RCC_BASE + 0x30))
#define RCC_APB1ENR						((u32 *) (RCC_BASE + 0x40))
#define RCC_APB2ENR						((u32 *) (RCC_BASE + 0x44))

#define RCC_CR_PLLSAIRDY				29
#define RCC_CR_PLLSAION					28
#define RCC_CR_PLLI2SRDY				27
#define RCC_CR_PLLI2SON					26
#define RCC_CR_PLLRDY					25
#define RCC_CR_PLLON					24
#define RCC_CR_CSSON					19
#define RCC_CR_HSEBYP					18
#define RCC_CR_HSERDY					17
#define RCC_CR_HSEON					16
#define RCC_CR_HSIRDY					1
#define RCC_CR_HSION					0

#define RCC_PLLCFGR_PLLSRC				22

#define RCC_PLLCFGR_PLLP_DIV2			0b00
#define RCC_PLLCFGR_PLLP_DIV4			0b01
#define RCC_PLLCFGR_PLLP_DIV6			0b10
#define RCC_PLLCFGR_PLLP_DIV8			0b11

#define RCC_PLLCFGR_PLLR(A)				(((A) & 0x7) << 28)
#define RCC_PLLCFGR_PLLQ(A)				(((A) & 0xF) << 24)
#define RCC_PLLCFGR_PLLP(A)				(((A) & 0x3) << 16)
#define RCC_PLLCFGR_PLLN(A)				(((A) & 0x1FF) << 6)
#define RCC_PLLCFGR_PLLM(A)				(((A) & 0x3F) << 0)

#define RCC_CFGR_PPRE_NDIV				0b000
#define RCC_CFGR_PPRE_DIV2				0b100
#define RCC_CFGR_PPRE_DIV4				0b101
#define RCC_CFGR_PPRE_DIV8				0b110
#define RCC_CFGR_PPRE_DIV16				0b111
#define RCC_CFGR_PPRE_RESET				0b111

#define RCC_CFGR_HPRE_NDIV				0b0000
#define RCC_CFGR_HPRE_DIV2				0b1000
#define RCC_CFGR_HPRE_DIV4				0b1001
#define RCC_CFGR_HPRE_DIV8				0b1010
#define RCC_CFGR_HPRE_DIV16				0b1011
#define RCC_CFGR_HPRE_DIV64				0b1100
#define RCC_CFGR_HPRE_DIV128			0b1101
#define RCC_CFGR_HPRE_DIV256			0b1110
#define RCC_CFGR_HPRE_DIV512			0b1111
#define RCC_CFGR_HPRE_RESET				0b1111

#define RCC_CFGR_PPRE2(A)				(((A) & 0x7) << 13)
#define RCC_CFGR_PPRE1(A)				(((A) & 0x7) << 10)
#define RCC_CFGR_HPRE(A)				(((A) & 0xF) << 4)

#define RCC_CFGR_SW_HSI					0b00
#define RCC_CFGR_SW_HSE					0b01
#define RCC_CFGR_SW_PLL_P				0b10
#define RCC_CFGR_SW_PLL_R				0b11
#define RCC_CFGR_SW_RESET				0b11
#define RCC_CFGR_SW(A)					(((A) & 0x3) << 0)
#define RCC_CFGR_SW_MASK				RCC_CFGR_SW(0b11)

#define RCC_AHB1ENR_GPIOAEN				0
#define RCC_AHB1ENR_GPIOBEN				1

#define RCC_APB2ENR_SPI1EN				12
#define RCC_APB2ENR_TIM11EN				18

#define RCC_APB1ENR_USART2EN			17

#endif
