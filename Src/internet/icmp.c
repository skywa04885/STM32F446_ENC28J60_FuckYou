#include "icmp.h"

u16 icmp_calc_cs(const icmp_pkt_t *pkt, u16 len)
{
	u16 sum = 0x0000;
	u16 n = (len % 2 == 0) ? len : len - 1;

	u16 *p = (u16 *) pkt;
	for (u16 i = 0; i < (n / 2); ++i)
		sum = checksum_oc16_add(sum, *p++);

	if (len % 2 != 0)
		sum = checksum_oc16_add(sum, (u16) ((u8 *) pkt)[len - 1]);

	return ~sum;
}
