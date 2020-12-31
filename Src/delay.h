#ifndef _SRC_DELAY_H
#define _SRC_DELAY_H

/*********************************************
 * Project Headers
 *********************************************/

#include "types.h"
#include "hardware/timers.h"
#include "hardware/rcc.h"

/*********************************************
 * Prototypes
 *********************************************/

void delay_init(void);

void delay_us(u16 us);
void delay_ms(u16 ms);
void delay_s(u16 s);

#endif
