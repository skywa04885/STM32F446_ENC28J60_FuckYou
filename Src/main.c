#include <stdio.h>

#include "internet/microchip/enc28j60.h"
#include "hardware/uart.h"
#include "hardware/systick.h"
#include "delay.h"
#include "internet/bswap.h"
#include "clock_init.h"

static u8 buffer[256];

static u8 dest[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

int main(void)
{
	// Initializes the peripherals
	clock_init();
	systick_init();
	delay_init();
	usart2_init(576000);

	// Initializes the ENC28J60
	enc28j60_spi_init();
	enc28j60_init();

	// Sets the LED modes
	enc28j60_ledb_mode(ENC28J60_PHLCON_LCFG_DISPLAY_TX_ACTIVITY);
	enc28j60_leda_mode(ENC28J60_PHLCON_LCFG_DISPLAY_RX_ACTIVITY);

	// Prints the MAC
	uint8_t mac[6];
	enc28j60_mac_read(mac);
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	// Prepares UDP packet
	const char *data = "Suck My Balls";

	// Creates the ethernet packet, with the IPv4 type
	enc28j60_pkt_t *pkt = (enc28j60_pkt_t *) buffer;
	enc28j60_pkt_prepare(pkt, dest, ETHERNET_PKT_TYPE_IPV4);

	// Creates the IP Packet
	ip_pkt_t *ip_pkt = (ip_pkt_t *) pkt->eth_pkt.payload;
	enc28j60_ipv4_prepare(ip_pkt, dest);

	// Creates the UDP Packet
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;
	udp_pkt_t *udp_pkt = (udp_pkt_t *) ip_ipv4->payload;
	enc28j60_ipv4_udp_prepare(ip_pkt, udp_pkt, 4004, data, strlen(data));

	// Finishes the packet and creates checksum
	enc28j60_ipv4_finish(ip_pkt);

	printf("%u\n", ip_pkt->hdr.tl);

    /* Loop forever */
	for(;;)
	{
		enc28j60_write(pkt, BSWAP16(ip_pkt->hdr.tl));
		for (uint32_t i = 0; i < 0xFFFFF7; ++i);
//		enc28j60_poll(buffer);
	}
}
