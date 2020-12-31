#ifndef _SRC_HARDWARE_FLASH_H
#define _SRC_HARDWARE_FLASH_H

#define FLASH_BASE						0x40023C00

#define RCC_ACR							((u32 *) (FLASH_BASE + 0x00))

#define FLASH_ACR_DCRST					12
#define FLASH_ACR_ICRST					11
#define FLASH_ACR_DCEN					10
#define FLASH_ACR_ICEN					9
#define FLASH_ACR_PRFTEN				8

#define FLASH_ACR_LATENCY(A)			(((A) & 0x7) << 0)

#endif
