#ifndef _SRC_HARDWARE_GPIO_H
#define _SRC_HARDWARE_GPIO_H

#include "../types.h"

#define GPIOA_BASE						0x40020000
#define GPIOB_BASE						0x40020400

#define GPIO_MODER(A)					((u32 *) ((A) + 0x00))
#define GPIO_OTYPER(A)					((u32 *) ((A) + 0x04))
#define GPIO_OSPEEDR(A)					((u32 *) ((A) + 0x08))
#define GPIO_PUPDR(A)					((u32 *) ((A) + 0x0C))
#define GPIO_IDR(A)						((u32 *) ((A) + 0x10))
#define GPIO_ODR(A)						((u32 *) ((A) + 0x14))
#define GPIO_AFRL(A)					((u32 *) ((A) + 0x20))
#define GPIO_AFRH(A)					((u32 *) ((A) + 0x24))

#define GPIO_MODE_INPUT					0b00
#define GPIO_MODE_OUTPUT				0b01
#define GPIO_MODE_AF					0b10
#define GPIO_MODE_ANALOG				0b11
#define GPIO_MODE_RESET					0b11

#define GPIO_MODE(A, B)					((B) << (A * 2))
#define GPIO_AF(A, B)					((B) << (A * 4))

#define GPIO_LOW_SPEED					0b00
#define GPIO_MED_SPEED					0b01
#define GPIO_FAST_SPEED					0b10
#define GPIO_HIGH_SPEED					0b11
#define GPIO_OSPEED(A, B)				((B) << (A * 2))

#endif
