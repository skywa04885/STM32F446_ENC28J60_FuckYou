#ifndef _SRC_HARDWARE_TIMERS_H
#define _SRC_HARDWARE_TIMERS_H

#include "../types.h"
#include "rcc.h"
#include "nvic.h"

#define TIM11_BASE							0x40014800
#define TIM9_BASE							0x40014000
#define TIM2_BASE							0x40000000

#define TIM_CR1(A)							((u16 *) ((A) + 0x00))
#define TIM_DIER(A)							((u16 *) ((A) + 0x04))
#define TIM_SR(A)							((u16 *) ((A) + 0x10))
#define TIM_EGR(A)							((u16 *) ((A) + 0x14))
#define TIM_CCMR1(A)							((u16 *) ((A) + 0x18))
#define TIM_CCER(A)							((u16 *) ((A) + 0x20))
#define TIM_CNT(A)							((u16 *) ((A) + 0x24))
#define TIM_PSC(A)							((u16 *) ((A) + 0x28))
#define TIM_ARR(A)							((u16 *) ((A) + 0x2C))
#define TIM_CCR1(A)							((u16 *) ((A) + 0x34))
#define TIM_OR(A)							((u16 *) ((A) + 0x50))

#define TIM_CR1_CEN							0
#define TIM_CR1_UDIS							1
#define TIM_CR1_URS							2

#define TIM_DIER_UIE							0

#define TIM_SR_UIF							0

#endif
