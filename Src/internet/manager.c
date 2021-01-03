#include "manager.h"

extern u8 buffer[];
extern u8 write_buffer[];

static u8 broadcast[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; /* May be used for IPv4 and MAC */
static u8 dhcp_cookie[4] = { 99, 130, 83, 99 };
static const char *label = "NetMan";

manager_config_t config = {
		/* State */
		.dhcp_state = MANAGER_DHCP_STATE_NOT_READY,
		/* Hardware Configuration */
		.max_frame_length = 512,
		.rx_buff_end = 4096,
		.tx_buff_start = 4098,
		/* Configuration */
		.mac = { 0xAA, 0xAA, 0xA4, 0x52, 0x37, 0x3C },
		.ipv4_address = { 0xFF, 0xFF, 0xFF, 0xFF },
		/* Servers */
		.ipv4_dns_server = { 8, 8, 8, 8 },
		/* Options */
		.full_duplex = 1,
		.ready = 0
};

void manager_send_dhcp_request(u8 *dhcp_server, u8 *addr)
{
	enc28j60_pkt_t *resp_pkt = (enc28j60_pkt_t *) write_buffer;
	ethernet_pkt_t *eth_resp_pkt = (ethernet_pkt_t *) &resp_pkt->eth_pkt;
	ip_pkt_t *ip_resp_pkt = (ip_pkt_t *) eth_resp_pkt->payload;

	// Prepares the BOOTP packet
	bootp_pkt_t *bootp_resp_pkt = pkt_builder_bootp((u8 *) &resp_pkt->eth_pkt, broadcast,
			broadcast, BOOTP_OPCODE_BOOTREQUEST, 60,
			_BV(BOOTP_FLAG_BROADCAST));

	// Sets the options, such as the message type, end and the addresses
	bootp_oparam_t *param = bootp_init_dhcp_options(bootp_resp_pkt);
	param = bootp_oparam_add_u8(BOOTP_OPTION_CODE_DHCP_MESSAGE_TYPE, DHCP_MESSAGE_TYPE_DHCPREQUEST, param);
	param = bootp_oparam_add_addr(BOOTP_OPTION_CODE_DHCP_SERVER_ID, dhcp_server, param);
	param = bootp_oparam_add_addr(BOOTP_OPTION_CODE_DHCP_REQUESTED_IP_ADDR, addr, param);
	param = bootp_oparam_end(param);

	// Finishes the BOOTP packet, writes it to the Ethernet module
	//  and update the DHCP state
	pkt_builder_bootp_finish(ip_resp_pkt, param);
	enc28j60_write(resp_pkt, BSWAP16(ip_resp_pkt->hdr.tl));
	config.dhcp_state = MANAGER_DHCP_STATE_REQUEST;
}

void manager_handle_dhcp(enc28j60_pkt_t *pkt)
{
	ip_pkt_t *ip_pkt = (ip_pkt_t *) pkt->eth_pkt.payload;
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;
	udp_pkt_t *udp_pkt = (udp_pkt_t *) ip_ipv4->payload;
	bootp_pkt_t *bootp_pkt = (bootp_pkt_t *) udp_pkt->payload;

	if (config.dhcp_state == MANAGER_DHCP_STATE_DISCOVER)
	{
		// Parses the given parameters, and sets the configuration variables if already
		//  possible, since the IPv4 one needs to wait for the ACK
		bootp_oparam_t *param = bootp_oparam_parser_init_dhcp(bootp_pkt);
		do
		{
			switch (param->code)
			{
			case BOOTP_OPTION_CODE_SUBNET_MASK:
				memcpy(config.ipv4_subnet_mask, ((bootp_oparam_addr_t *) param->body)->addr, 4);
				break;
			case BOOTP_OPTION_CODE_DHCP_SERVER_ID:
				memcpy(config.ipv4_dhcp_server, ((bootp_oparam_addr_t *) param->body)->addr, 4);
				break;
			case BOOTP_OPTION_CODE_ROUTER_OPTION:
				memcpy(config.ipv4_router, ((bootp_oparam_addr_t *) param->body)->addr, 4);
				break;
			default: break;
			}
		} while ((param = bootp_oparam_parser_next(param)) != NULL);

		// Sends the DHCP Request
		manager_send_dhcp_request(config.ipv4_dhcp_server, bootp_pkt->body.yiaddr);
	} else if (config.dhcp_state == MANAGER_DHCP_STATE_REQUEST)
	{
		//
		// Parses the DHCP Server Response, and applies the settings
		//

		bool dhcp_accepted = false;

		// Parses the options, and gets the message type
		bootp_oparam_t *param = bootp_oparam_parser_init_dhcp(bootp_pkt);
		do
		{
			if (param->code != BOOTP_OPTION_CODE_DHCP_MESSAGE_TYPE) continue;

			if (BOOTP_OPARAM_U8(param)->val == DHCP_MESSAGE_TYPE_DHCPACK) dhcp_accepted = true;
			else dhcp_accepted = false;

			break;
		} while ((param = bootp_oparam_parser_next(param)) != NULL);

		// Checks if the DHCP server sent ACK, if not, resend the DHCP
		//  request and return from this function-
		if (!dhcp_accepted)
		{
			manager_dhcp_init();
			return;
		}

		// Copies our assigned IPv4 address into the config, and sets the
		//  ready flag
		memcpy(config.ipv4_address, bootp_pkt->body.yiaddr, 4);
		config.ready = 1;

		//
		// Sends the DNS stuff
		//

		manager_dns_resolve(buffer, "fannst.nl");
	}
}

extern void udp_packet_callback(enc28j60_pkt_t *pkt)
{
	ip_pkt_t *ip_pkt = (ip_pkt_t *) pkt->eth_pkt.payload;
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;
	udp_pkt_t *udp_pkt = (udp_pkt_t *) ip_ipv4->payload;

	switch (BSWAP16(udp_pkt->hdr.dp))
	{
		case 68:
		{
			// Checks if we should do anything with the request
			if (config.ready) return;

			// Reads the packet as if it is a BOOTP one, and checks if
			//  it contains the 'magic cookie', and no.. It's not weed!
			bootp_pkt_t *bootp_pkt = (bootp_pkt_t *) udp_pkt->payload;
			if (memcmp(bootp_pkt->body.payload, dhcp_cookie, 4) != 0) return;

			// Calls the DHCP handler
			manager_handle_dhcp(pkt);
			break;
		}
		default: break;
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
}

void manager_dhcp_send_bootp_request()
{
	enc28j60_pkt_t *pkt = (enc28j60_pkt_t *) buffer;
	ethernet_pkt_t *eth_pkt = (ethernet_pkt_t *) &pkt->eth_pkt;
	ip_pkt_t *ip_pkt = (ip_pkt_t *) eth_pkt->payload;

	// Prepares the BOOTP packet
	bootp_pkt_t *bootp_pkt = pkt_builder_bootp((u8 *) &pkt->eth_pkt, broadcast,
			broadcast, BOOTP_OPCODE_BOOTREQUEST, 60,
			_BV(BOOTP_FLAG_BROADCAST));

	// Sets the BOOTP options, such as the type, end and the message type
	bootp_oparam_t *param = bootp_init_dhcp_options(bootp_pkt);
	param = bootp_oparam_add_u8(BOOTP_OPTION_CODE_DHCP_MESSAGE_TYPE, DHCP_MESSAGE_TYPE_DHCPDISCOVER, param);
	param = bootp_oparam_end(param);

	// Finishes the packet ( Adding checksum ), and sends it to the ENC28J60
	pkt_builder_bootp_finish(ip_pkt, param);
	enc28j60_write(pkt, BSWAP16(ip_pkt->hdr.tl));
}

void manager_dhcp_init(void)
{
	manager_dhcp_send_bootp_request();
	config.dhcp_state = MANAGER_DHCP_STATE_DISCOVER;
}

void manager_poll(void)
{
	static bool link_up = true;

	// Checks the link status, if it is down, we want to stop polling
	//  since it is useless
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

	// Polls the ENC28J60 for any events
	enc28j60_poll(buffer);
}

/*********************************************
 * Prototypes ( UDP )
 *********************************************/

u8 manager_dns_resolve(u8 *buffer, const char *hostname)
{
	u8 n = 0;

	//
	// Sends the DNS Request
	//

	dns_pkt_t *dns_pkt = pkt_builder_dns(write_buffer, config.mac_dns_server,
			config.ipv4_dns_server, 60, 0x0000);

	dns_pkt->hdr.qr = 0;
	dns_pkt->hdr.opcode = DNS_OPCODE_QUERY;
	dns_pkt->hdr.aa = 0;
	dns_pkt->hdr.tc = 0;
	dns_pkt->hdr.rd = 0;
	dns_pkt->hdr.ra = 0;
	dns_pkt->hdr.z = 0;
	dns_pkt->hdr.rcode = 0;

	dns_pkt->hdr.qdcnt = 1;
	dns_pkt->hdr.ancnt = 0;
	dns_pkt->hdr.nscnt = 0;
	dns_pkt->hdr.arcnt = 0;

	dns_add_question(dns_pkt->payload, hostname, DNS_RR_ALL, DNS_QCLASS_ANY);

	//
	// Reads the DNS response
	//

	dns_label_seg_t *seg = (dns_label_seg_t *) dns_pkt->payload;
	do
	{
		for (u8 i = 0; i < seg->len; ++i)
			printf("%c", seg->next[i]);
		printf("\n");
	} while ((seg = dns_label_parser_next(seg)) != NULL);

	return n;
}
