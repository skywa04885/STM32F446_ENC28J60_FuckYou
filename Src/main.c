#include <stdio.h>
#include <malloc.h>

#include "internet/microchip/enc28j60.h"
#include "hardware/uart.h"
#include "delay.h"
#include "internet/bswap.h"
#include "clock_init.h"

u8 buffer[4096];

static u8 dest[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

int main(void)
{
	// Initializes the peripherals
	clock_init();
	delay_init();
	usart2_init(500000);

	// Initializes the network manager
	manager_init();
	manager_dhcp_init();

    /* Loop forever */
	for(;;)
	{
		manager_poll();
	}
}
