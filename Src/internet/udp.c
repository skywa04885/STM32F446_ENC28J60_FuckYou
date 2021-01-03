#include "udp.h"

u16 udp_calc_cs(const udp_pkt_t *udp_pkt, const u8 *da, const u8 *sa, u8 sal, u8 dal, u8 proto)
{
	u16 sum = 0x0000, *p;

	// Adds the pseudo header to the sum

	p = (u16 *) sa;
	for (u8 i = 0; i < (sal / 2); ++i)
		sum = checksum_oc16_add(sum, p[i]);

	p = (u16 *) da;
	for (u8 i = 0; i < (dal / 2); ++i)
		sum = checksum_oc16_add(sum, p[i]);

	sum = checksum_oc16_add(sum, BSWAP16((u16) proto));
	sum = checksum_oc16_add(sum, udp_pkt->hdr.l);

	// Adds the UDP header to the sum
	p = (u16 *) &udp_pkt->hdr;
	for (u8 i = 0; i < (sizeof (udp_hdr_t) / 2); ++i)
		sum = checksum_oc16_add(sum, p[i]);

	// Adds the data to the sum
	p = (u16 *) udp_pkt->payload;
	u16 l = BSWAP16(udp_pkt->hdr.l),
			n = (l % 2 == 0) ? l : l - 1;

	for (u16 i = 0; i < (BSWAP16(udp_pkt->hdr.l) / 2); ++i)
		sum = checksum_oc16_add(sum, p[i]);

	if (l % 2 != 0)
		sum = checksum_oc16_add(sum, (u16) udp_pkt->payload[n - 1]);

	return ~sum;
}
