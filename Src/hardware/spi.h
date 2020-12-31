#ifndef _SRC_HARDWARE_SPI_H
#define _SRC_HARDWARE_SPI_H

#include "../types.h"

#define SPI1_BASE				0x40013000

#define SPI_CR1(A)				((u16 *) ((A) + 0x00))
#define SPI_CR2(A)				((u16 *) ((A) + 0x04))
#define SPI_SR(A)				((u16 *) ((A) + 0x08))
#define SPI_DR(A)				((u8 *) ((A) + 0x0C))

#define SPI_CR1_BIDIMODE		15
#define SPI_CR1_BIDIOE			14
#define SPI_CR1_CRCEN			13
#define SPI_CR1_CRCNEXT			12
#define SPI_CR1_DFF				11
#define SPI_CR1_RXONLY			10
#define SPI_CR1_SSM				9
#define SPI_CR1_SSI				8
#define SPI_CR1_LSBFIRST		7
#define SPI_CR1_SPE				6
#define SPI_CR1_MSTR			2
#define SPI_CR1_CPOL			1
#define SPI_CR1_CPHA			0

#define SPI_CR1_BR_DIV2			0b000
#define SPI_CR1_BR_DIV4			0b001
#define SPI_CR1_BR_DIV8			0b010
#define SPI_CR1_BR_DIV16		0b011
#define SPI_CR1_BR_DIV32		0b100
#define SPI_CR1_BR_DIV64		0b101
#define SPI_CR1_BR_DIV128		0b110
#define SPI_CR1_BR_DIV256		0b111
#define SPI_CR1_BR(A)			((A) << 3)

#define SPI_CR2_TXEIE			7
#define SPI_CR2_RXNEIE			6
#define SPI_CR2_ERRIE			5
#define SPI_CR2_FRF				4
#define SPI_CR2_SSOE			2
#define SPI_CR2_TXDMAEN			1
#define SPI_CR2_RXDMAEN			0

#define SPI_SR_FRE				8
#define SPI_SR_BSY				7
#define SPI_SR_OVR				6
#define SPI_SR_MODF				5
#define SPI_SR_CRCERR			4
#define SPI_SR_UDR				3
#define SPI_SR_CHSIDE			2
#define SPI_SR_TXE				1
#define SPI_SR_RXNE				0

#endif
