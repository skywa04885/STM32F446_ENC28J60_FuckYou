#ifndef _SRC_INTERNET_ETHERNET_H
#define _SRC_INTERNET_ETHERNET_H

/*********************************************
 * Project Headers
 *********************************************/

#include "../types.h"

/*********************************************
 * Data Types
 *********************************************/

#define ETHERNET_PKT_TYPE_IPV4			0x0800
#define ETHERNET_PKT_TYPE_ARP			0x0806

typedef struct __attribute__ (( packed ))
{
	u8 da[6];
	u8 sa[6];
	u16 type;
} ethernet_hdr_t;

typedef struct __attribute__ (( packed ))
{
	ethernet_hdr_t hdr;
	u8 payload[0];
} ethernet_pkt_t;

#endif
