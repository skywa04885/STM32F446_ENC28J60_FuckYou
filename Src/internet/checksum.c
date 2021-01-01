#include "checksum.h"

u16 checksum_oc16_add(u16 sum, u16 val)
{
	if ((u32) sum + (u32) val >= 0xFFFF)
		++sum;

	sum += val;
	return sum;
}

u16 checksum_oc16(u16 *data, u16 len)
{
	u16 sum = 0x0000;

	for (u16 i = 0; i < len; ++i)
		sum = checksum_oc16_add(sum, *data++);

	return ~sum;
}
