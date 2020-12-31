#ifndef _SRC_HARDWARE_UART_H
#define _SRC_HARDWARE_UART_H

#include "../types.h"
#include "gpio.h"
#include "rcc.h"

/*********************************************
 * Registers
 *********************************************/

#define USART2_BASE								0x40004400

#define USART_SR(A)								((u32 *)((A) + 0x00))
#define USART_DR(A)								((u32 *)((A) + 0x04))
#define USART_BRR(A)							((u32 *)((A) + 0x08))
#define USART_CR1(A)							((u32 *)((A) + 0x0C))
#define USART_CR2(A)							((u32 *)((A) + 0x10))
#define USART_CR3(A)							((u32 *)((A) + 0x14))
#define USART_GTPR(A)							((u32 *)((A) + 0x18))

#define USART_CR1_OVER8							15
#define USART_CR1_UE							13
#define USART_CR1_M								12
#define USART_CR1_WAKE							11
#define USART_CR1_PCE							10
#define USART_CR1_PS							9
#define USART_CR1_PEIE							8
#define USART_CR1_TXEIE							7
#define USART_CR1_TCIE							6
#define USART_CR1_RXNEIE						5
#define USART_CR1_IDLEIE						4
#define USART_CR1_TE							3
#define USART_CR1_RE							2
#define USART_CR1_RWU							1
#define USART_CR1_BRK							0

#define USART_SR_CTS							9
#define USART_SR_LBD							8
#define USART_SR_TXE							7
#define USART_SR_TC								6
#define USART_SR_RXNE							5
#define USART_SR_IDLE							4
#define USART_SR_ORE							3
#define USART_SR_NF								2
#define USART_SR_FE								1
#define USART_SR_PE								0

/*********************************************
 * Prototypes
 *********************************************/

void usart2_init(u32 baud);

void usart2_write_char(char c);
char usart2_read_char(void);

#endif
