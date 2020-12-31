#include "uart.h"

void usart2_init(u32 baud)
{
	// Enables the clock for GPIOA and USART2
	*RCC_AHB1ENR |= (_BV(RCC_AHB1ENR_GPIOAEN));
	*RCC_APB1ENR |= (_BV(RCC_APB1ENR_USART2EN));

	// Configures the USART2 GPIO
	*GPIO_MODER(GPIOA_BASE) &= ~(GPIO_MODE(2, GPIO_MODE_RESET) | GPIO_MODE(3, GPIO_MODE_RESET));

	*GPIO_MODER(GPIOA_BASE) |= (GPIO_MODE(2, GPIO_MODE_AF) | GPIO_MODE(3, GPIO_MODE_AF));

	*GPIO_AFRL(GPIOA_BASE) |= (GPIO_AF(2, 7) | GPIO_AF(3, 7));

	// Configures the USART2 Peripheral
	*USART_BRR(USART2_BASE) = (((2 * 45000000) + baud) / (2 * baud));

	*USART_CR1(USART2_BASE) |= (_BV(USART_CR1_TE) | _BV(USART_CR1_RE) |
			_BV(USART_CR1_UE));
}

void usart2_write_char(char c)
{
	if (c == '\n') usart2_write_char('\r');
	while (!(*USART_SR(USART2_BASE) & _BV(USART_SR_TXE)));
	*USART_DR(USART2_BASE) = (u16) c;
}

char usart2_read_char(void)
{
	while (!(*USART_SR(USART2_BASE) & _BV(USART_SR_RXNE)));
	return (char) *USART_DR(USART2_BASE);
}

int __io_putchar(int c)
{
	usart2_write_char((char) c);
	return c;
}

int __io_getchar(void)
{
	return (int) usart2_read_char;
}
