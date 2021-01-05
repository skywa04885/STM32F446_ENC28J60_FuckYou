#include "discover.h"

static u8 __discover_pkt_prefix[3] = { 0x67, 0x61, 0x79 };

u8 *discover_pkt_prefix()
{
	return __discover_pkt_prefix;
}
