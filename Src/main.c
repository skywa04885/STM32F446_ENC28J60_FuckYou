#include <stdio.h>

#include "internet/microchip/enc28j60.h"
#include "hardware/uart.h"
#include "delay.h"
#include "clock_init.h"

static u8 buffer[256];

static u8 dest[6] = { 0x12, 0x24, 0x11, 0x22, 0x18, 0x44 };

int main(void)
{
	// Initializes the peripherals
	clock_init();
	delay_init();
	usart2_init(576000);

	// Initializes the ENC28J60
	enc28j60_spi_init();
	enc28j60_init();

	// Sets the LED modes
	enc28j60_ledb_mode(ENC28J60_PHLCON_LCFG_DISPLAY_TX_ACTIVITY);
	enc28j60_leda_mode(ENC28J60_PHLCON_LCFG_DISPLAY_LINK_STATUS_AND_RX_ACTIVITY);

	// Prints the MAC
	uint8_t mac[6];
	enc28j60_mac_read(mac);
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    /* Loop forever */
	const char *data = "Hello World\n";
	enc28j60_pkt_t *pkt = (enc28j60_pkt_t *) buffer;
	enc28j60_pkt_prepare(pkt, dest, ETHERNET_PKT_TYPE_IPV4);
	memcpy(pkt->eth_pkt.payload, data, strlen(data));
	for(;;)
	{
		enc28j60_write(pkt, strlen(data));
		for (uint32_t i = 0; i < 0xFFFFF; ++i);
	}
}
