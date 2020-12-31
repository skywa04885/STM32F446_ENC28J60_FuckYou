#ifndef _SRC_CLOCK_INIT_H
#define _SRC_CLOCK_INIT_H

/*********************************************
 * Project Headers
 *********************************************/

#include "hardware/rcc.h"
#include "hardware/flash.h"

/*********************************************
 * Prototypes
 *********************************************/

/**
 * Initializes the clock in the STM32F4RE
 */
void clock_init(void);

#endif
