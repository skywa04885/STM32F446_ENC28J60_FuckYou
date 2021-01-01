#ifndef _SRC_HARDWRE_SYSTICK_H
#define _SRC_HARDWRE_SYSTICK_H

#include "../types.h"

/*********************************************
 * Systick Registers
 *********************************************/

#define STK_CTRL								((u32 *) 0xE000E010)
#define STK_LOAD								((u32 *) 0xE000E014)
#define STK_VAL								((u32 *) 0xE000E018)
#define STK_CALIB							((u32 *) 0xE000E01C)

#define STK_CTRL_COUNTFLAG					16
#define STK_CTRL_CLKSOURCE					2
#define STK_CTRL_TICKINT						1
#define STK_CTRL_ENABLE						0

/*********************************************
 * Systick Definitions
 *********************************************/

#define STK_LOAD_VALUE						(1000000 * 30)
#define STK_CALIB_VALUE						0

/*********************************************
 * Systick Prototypes
 *********************************************/

void 	systick_init(void);
u64 	systick_get_time(void);

#endif
