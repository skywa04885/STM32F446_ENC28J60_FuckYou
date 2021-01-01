#ifndef _SRC_INTERNET_ARP_H
#define _SRC_INTERNET_ARP_H

/*********************************************
 * Project Headers
 *********************************************/

#include "../types.h"

/*********************************************
 * DataTypes
 *********************************************/

#define ARP_PKT_OP_REQUEST					1
#define ARP_PKT_OP_REPLY						2

typedef struct __attribute__ (( packed ))
{
	u16			hrd;		/* Hardware Address Space */
	u16			pro;		/* Protocol Address Space */
	u8			hln;		/* Byte-Length of Hardware Address */
	u8			pln;		/* Byte-Length of Protocol Address */
	u16			op;			/* Opcode */
} arp_hdr_t;

typedef struct __attribute__ (( packed ))
{
	u8			sha[6];		/* Hardware Address of sender */
	u8			spa[4];		/* Protocol Address of sender */
	u8			tha[6];		/* Hardware Address of target */
	u8			tpa[4];		/* Protocol Address of target */
} arp_payload_ipv4_t;

typedef struct __attribute__ (( packed ))
{
	u8			sha[6];		/* Hardware Address of sender */
	u8			spa[16];	/* Protocol Address of sender */
	u8			tha[6];		/* Hardware Address of target */
	u8			tpa[16];	/* Protocol Address of target */
} arp_payload_ipv6_t;

typedef struct __attribute__ (( packed ))
{
	arp_hdr_t 	hdr;
	u8			payload[0];
} arp_pkt_t;

#endif
