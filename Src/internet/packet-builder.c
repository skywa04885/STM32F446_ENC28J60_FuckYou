#include "packet-builder.h"

extern manager_config_t config;

ethernet_pkt_t *pkt_builder_ethernet(u8 *buffer, u8 *ha, u16 proto)
{
	ethernet_pkt_t *eth_pkt = (ethernet_pkt_t *) buffer;

	memcpy(eth_pkt->hdr.da, ha, 6);
	memcpy(eth_pkt->hdr.sa, config.mac, 6);

	eth_pkt->hdr.type = BSWAP16(proto);

	return eth_pkt;
}

ip_pkt_t *pkt_builder_ip(u8 *buffer, u8 *ha, u8 *dest4, u8 ttl, u8 proto)
{
	ethernet_pkt_t *eth_pkt = pkt_builder_ethernet(buffer, ha, ETHERNET_PKT_TYPE_IPV4);
	ip_pkt_t *ip_pkt = (ip_pkt_t *) eth_pkt->payload;
	ip_ipv4_body_t *ipv4_body = (ip_ipv4_body_t *) ip_pkt->payload;

	memcpy(ipv4_body->da, dest4, 4);
	memcpy(ipv4_body->sa, config.ipv4_address, 4);

	ip_pkt->hdr.f = BSWAP16(0x00);
	ip_pkt->hdr.id = BSWAP16(0x00);

	ip_pkt->hdr.proto = proto;
	ip_pkt->hdr.ttl = ttl;
	ip_pkt->hdr.ihl = 5; 		/* IPv4 Packet Size */
	ip_pkt->hdr.ver = 4;		/* IPv4 Version */
	ip_pkt->hdr.tos = (IP_HDR_TOS_PRECEDENCE(IP_HDR_TOS_PRECEDENCE_ROUTINE));

	return ip_pkt;
}

udp_pkt_t *pkt_builder_udp(u8 *buffer, u8 *ha, u8 *dest4, u8 ttl, u16 port)
{
	ip_pkt_t *ip_pkt = pkt_builder_ip(buffer, ha, dest4, ttl, IP_HDR_PROTO_UDP);
	ip_ipv4_body_t *ipv4_body = (ip_ipv4_body_t *) ip_pkt->payload;
	udp_pkt_t *udp_pkt = (udp_pkt_t *) ipv4_body->payload;

	udp_pkt->hdr.sp = BSWAP16(0);
	udp_pkt->hdr.dp = BSWAP16(port);

	return udp_pkt;
}

bootp_pkt_t *pkt_builder_bootp(u8 *buffer, u8 *ha, u8 *dest4, u8 op, u8 ttl, u16 flags)
{
	udp_pkt_t *udp_pkt = pkt_builder_udp(buffer, ha, dest4, ttl, BOOTP_UDP_SERVER_PORT);
	bootp_pkt_t *bootp_pkt = (bootp_pkt_t *) udp_pkt->payload;

	memcpy(bootp_pkt->body.chaddr, config.mac, 6);

	bootp_pkt->hdr.hlen = 6;
	bootp_pkt->hdr.op = op;
	bootp_pkt->hdr.flags = BSWAP16(flags);

	return bootp_pkt;
}

dns_pkt_t *pkt_builder_dns(u8 *buffer, u8 *ha, u8 *dest4, u8 ttl, u16 id)
{
	udp_pkt_t *udp_pkt = pkt_builder_udp(buffer, ha, dest4, ttl, DNS_PORT);
	dns_pkt_t *dns_pkt = (dns_pkt_t *) udp_pkt->payload;

	dns_pkt->hdr.id = id;

	return dns_pkt;
}

arp_pkt_t *pkt_builder_arp(u8 *buffer, u8 *ha, u8 *tha, u8 *tpa, u8 ttl, u16 op)
{
	ethernet_pkt_t *eth_pkt = pkt_builder_ethernet(buffer, ha, ETHERNET_PKT_TYPE_ARP);
	arp_pkt_t *arp_pkt = (arp_pkt_t *) eth_pkt->payload;
	arp_payload_ipv4_t *arp_ipv4 = (arp_payload_ipv4_t *) arp_pkt->payload;

	arp_pkt->hdr.hln = 6;
	arp_pkt->hdr.pln = 4;
	arp_pkt->hdr.hrd = BSWAP16(1);
	arp_pkt->hdr.pro = BSWAP16(ETHERNET_PKT_TYPE_IPV4);
	arp_pkt->hdr.op = BSWAP16(op);

	memcpy(arp_ipv4->sha, config.mac, 6);
	memcpy(arp_ipv4->spa, config.ipv4_address, 4);

	memcpy(arp_ipv4->tha, tha, 6);
	memcpy(arp_ipv4->tpa, tpa, 4);

	return arp_pkt;
}

void pkt_builder_ip_finish(ip_pkt_t *ip_pkt)
{
	ip_pkt->hdr.cs = 0;
	ip_pkt->hdr.cs = checksum_oc16((u16 *) ip_pkt, (ip_pkt->hdr.ihl * 2));
}

void pkt_builder_udp_finish(ip_pkt_t *ip_pkt, udp_pkt_t *udp_pkt)
{
	ip_ipv4_body_t *ipv4_body = (ip_ipv4_body_t *) ip_pkt->payload;

	udp_pkt->hdr.cs = 0;
	udp_pkt->hdr.cs = udp_calc_cs(udp_pkt, ipv4_body->da, ipv4_body->sa, 4, 4, IP_HDR_PROTO_UDP);
}

void pkt_builder_bootp_finish(ip_pkt_t *ip_pkt, bootp_oparam_t *lparam)
{
	ip_ipv4_body_t *ipv4_body = (ip_ipv4_body_t *) ip_pkt->payload;
	udp_pkt_t *udp_pkt = (udp_pkt_t *) ipv4_body->payload;\
	bootp_pkt_t *bootp_pkt = (bootp_pkt_t *) udp_pkt->payload;

	// Calculates the UDP Packet length, by doing: size udp_pkt_t + size bootp_pkt_t + (options start - options end)
	udp_pkt->hdr.l = BSWAP16(sizeof (udp_pkt_t) + sizeof (bootp_pkt_t) + (((u8 *) lparam) - bootp_pkt->body.payload));
	ip_pkt->hdr.tl = BSWAP16((ip_pkt->hdr.ihl * 4) + sizeof (udp_pkt_t) + BSWAP16(udp_pkt->hdr.l));

	// Creates the checksums
	pkt_builder_udp_finish(ip_pkt, udp_pkt);
	pkt_builder_ip_finish(ip_pkt);
}
