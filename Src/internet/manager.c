#include "manager.h"

extern u8 buffer[];

static u8 broadcast[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; /* May be used for IPv4 and MAC */
static u8 dhcp_cookie[4] = { 99, 130, 83, 99 };
static const char *label = "NetMan";

manager_config_t config = {
		.dhcp_state = MANAGER_DHCP_STATE_NOT_READY,
		.max_frame_length = 512,
		.rx_buff_end = 4096,
		.tx_buff_start = 4098,
		.mac = { 0xAA, 0xAA, 0xA4, 0x52, 0x37, 0x3C },
		.ipv4 = { 0xFF, 0xFF, 0xFF, 0xFF },
		.full_duplex = 1,
		.ready = 0
};

extern void udp_packet_callback(enc28j60_pkt_t *pkt)
{
	ip_pkt_t *ip_pkt = (ip_pkt_t *) pkt->eth_pkt.payload;
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;
	udp_pkt_t *udp_pkt = (udp_pkt_t *) ip_ipv4->payload;

	switch (BSWAP16(udp_pkt->hdr.dp))
	{
		case 68:
		{
			if (config.ready) return;
			bootp_pkt_t *bootp_pkt = (bootp_pkt_t *) udp_pkt->payload;

			// Checks if the BOOTP Packet is DHCP
			if (memcmp(bootp_pkt->body.payload, dhcp_cookie, 4) != 0) return;

			LOGGER_INFO(label, "BOOTP/DHCP Responded\n");

			switch (config.dhcp_state)
			{
				case MANAGER_DHCP_STATE_DISCOVER:
				{
					LOGGER_INFO(label, "DHCP Offers: %u.%u.%u.%u, requesting ..\n",
							bootp_pkt->body.yiaddr[0],
							bootp_pkt->body.yiaddr[1],
							bootp_pkt->body.yiaddr[2],
							bootp_pkt->body.yiaddr[3]);

					// Parses the options
					bootp_oparam_t *param = (bootp_oparam_t *) (bootp_pkt->body.payload + 4);

					u8 c = 0;
					while (param->code != BOOTP_OPTION_CODE_END || c++ >= 255)
					{
						if (param->code == BOOTP_OPTION_CODE_PAD)
						{
							++param;
							continue;
						}

						switch (param->code)
						{
							case BOOTP_OPTION_CODE_SUBNET_MASK:
							{
								bootp_oparam_addr_t *addr = (bootp_oparam_addr_t *) param->body;
								printf("SUBNET Mask: %u.%u.%u.%u\n", addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3]);
								break;
							}
							case BOOTP_OPTION_CODE_DHCP_SERVER_ID:
							{
								bootp_oparam_addr_t *addr = (bootp_oparam_addr_t *) param->body;
								printf("DHCP Server: %u.%u.%u.%u\n", addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3]);
								break;
							}
							case BOOTP_OPTION_CODE_DNS_SERVER:
							{
								bootp_oparam_addr_t *addr = (bootp_oparam_addr_t *) param->body;
								printf("DNS Server: %u.%u.%u.%u\n", addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3]);
								break;
							}
							case BOOTP_OPTION_CODE_ROUTER_OPTION:
							{
								bootp_oparam_addr_t *addr = (bootp_oparam_addr_t *) param->body;
								printf("Router: %u.%u.%u.%u\n", addr->addr[0], addr->addr[1], addr->addr[2], addr->addr[3]);
								break;
							}
							default: break;
						}

						// Goes to the next option
						param = (bootp_oparam_t *) &param->body[param->len];
					}

					break;
				}
				case MANAGER_DHCP_STATE_REQUEST:
				{
					break;
				}
				default: break;
			}
			break;
		}
	}
}

void manager_init(void)
{
	// Initializes the ENC28J60
	enc28j60_spi_init();
	enc28j60_init();

	// Sets ENC28J60 LED modes
	enc28j60_ledb_mode(ENC28J60_PHLCON_LCFG_DISPLAY_TX_ACTIVITY);
	enc28j60_leda_mode(ENC28J60_PHLCON_LCFG_DISPLAY_RX_ACTIVITY);

	// Waits for the link
	LOGGER_INFO(label, "Awaiting LINK\n");
	while (!enc28j60_is_link_up());
	LOGGER_INFO(label, "LINK Active\n");

	// Prints the MAC
	enc28j60_mac_read(buffer);
	LOGGER_INFO(label, "Device MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
}

void manager_dhcp_send_bootp_request()
{
	LOGGER_INFO(label, "Sending DHCP request\n");

	enc28j60_pkt_t *pkt = (enc28j60_pkt_t *) buffer;
	ip_pkt_t *ip_pkt = (ip_pkt_t *) pkt->eth_pkt.payload;
	ip_ipv4_body_t *ipv4_body = (ip_ipv4_body_t *) ip_pkt->payload;
	udp_pkt_t *udp_pkt = (udp_pkt_t *) ipv4_body->payload;

	// Gets the pointer to the BOOTP packet
	bootp_pkt_t *bootp_pkt = (bootp_pkt_t *) udp_pkt->payload;

	// Prepares the BOOTP packet itself
	bootp_pkt->hdr.op = BOOTP_OPCODE_BOOTREQUEST;
	bootp_pkt->hdr.hlen = 6;
	bootp_pkt->hdr.flags = BSWAP16(_BV(BOOTP_FLAG_BROADCAST));
	memcpy(bootp_pkt->body.chaddr, config.mac, 6);

	// Gets the pointer to the current option
	bootp_oparam_t *param = NULL;

	// Sets the magic cookie on the BOOTP options, this will indicate
	//  that we're using the DHCP extension
	{
		bootp_oparam_dhcp_cookie_t *cookie = (bootp_oparam_dhcp_cookie_t *) bootp_pkt->body.payload;
		memcpy(cookie->values, dhcp_cookie, 4);
		param = (bootp_oparam_t *) cookie->next;
	}

	// Sets the DHCP message type
	param->code = BOOTP_OPTION_CODE_DHCP_MESSAGE_TYPE;
	param->len = 1;
	BOOTP_OPARAM_U8(param->body)->val = DHCP_MESSAGE_TYPE_DHCPDISCOVER;
	param = BOOTP_OPARAM_U8_NEXT(param->body);

	// Calculates the UDP Packet length, by doing: size udp_pkt_t + size bootp_pkt_t + (options start - options end)
	udp_pkt->hdr.l = BSWAP16(sizeof (udp_pkt_t) + sizeof (bootp_pkt_t) + (((u8 *) param) - bootp_pkt->body.payload));

	/*
		 Sends the packet
	*/

	enc28j60_pkt_prepare(pkt, broadcast, ETHERNET_PKT_TYPE_IPV4);
	enc28j60_ipv4_prepare(ip_pkt, broadcast);
	enc28j60_ipv4_udp_prepare(ip_pkt, udp_pkt, BOOTP_UDP_SERVER_PORT);
	enc28j60_ipv4_finish(ip_pkt);

	enc28j60_write(pkt, BSWAP16(ip_pkt->hdr.tl));
}

void manager_dhcp_init(void)
{
	enc28j60_pkt_t *pkt = (enc28j60_pkt_t *) buffer;
	ethernet_pkt_t *eth_pkt = &pkt->eth_pkt;
	ip_pkt_t *ip_pkt = (ip_pkt_t *) eth_pkt->payload;
	ip_ipv4_body_t *ipv4_body = (ip_ipv4_body_t *) ip_pkt->payload;
	udp_pkt_t *udp_pkt = (udp_pkt_t *) ipv4_body->payload;
	bootp_pkt_t *bootp_pkt = (bootp_pkt_t *) udp_pkt->payload;

	// Sends the BOOTP request
	manager_dhcp_send_bootp_request();
	config.dhcp_state = MANAGER_DHCP_STATE_DISCOVER;
}

static bool link_up = true;

void manager_poll(void)
{
	if (!enc28j60_is_link_up())
	{
		if (link_up)
		{
			LOGGER_INFO(label, "LINK down\n");
			link_up = false;
		}

		for (u16 i = 0; i < 0xFFFF; ++i);

		return;
	} else if (!link_up)
	{
		LOGGER_INFO(label, "LINK UP\n");
		link_up = true;
	}

	enc28j60_poll(buffer);
}
