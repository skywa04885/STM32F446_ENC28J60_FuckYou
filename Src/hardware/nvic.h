#ifndef _SRC_HARDWARE_NVIC_H
#define _SRC_HARDWARE_NVIC_H

#include "../types.h"

#define NBIC_ISER_BASE							((u32 *) 0xE000E100)
#define NVIC_ICER_BASE							((u32 *) 0xE000E180)
#define NVIC_ISPR_BASE							((u32 *) 0xE000E200)
#define NVIC_ICPR_BASE							((u32 *) 0xE000E280)
#define NVIC_IABR_BASE							((u32 *) 0xE000E300)
#define NVIC_IPR_BASE							((u32 *) 0xE000E400)

void nvic_enable_interrupt(u32 n);

#endif
