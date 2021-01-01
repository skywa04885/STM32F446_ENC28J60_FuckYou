#ifndef _SRC_INTERNET_CHECKSUM_H
#define _SRC_INTERNET_CHECKSUM_H

/*********************************************
 * Project Headers
 *********************************************/

#include "../types.h"

/*********************************************
 * Prototypes
 *********************************************/

u16		checksum_oc16_add(u16 sum, u16 val);
u16		checksum_oc16(u16 *data, u16 len);

#endif
