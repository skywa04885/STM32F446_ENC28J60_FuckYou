#ifndef _SRC_INTERNET_IP_H
#define _SRC_INTERNET_IP_H

/*********************************************
 * Project Headers
 *********************************************/

#include "../types.h"

/*********************************************
 * Data Types
 *********************************************/

#define IP_HDR_PROTO_ICMP					1
#define IP_HDR_PROTO_UDP						17
#define IP_HDR_PROTO_TCP						6

#define IP_HDR_TOS_PRECEDENCE_ROUTINE		0
#define IP_HDR_TOS_PRECEDENCE_PRIORITY		1
#define IP_HDR_TOS_PRECEDENCE_IMMEDIATE		2

#define IP_HDR_TOS_PRECEDENCE(A)				(((A) & 0x7) << 0)

typedef struct __attribute__ (( packed ))
{
	unsigned	ihl		: 4;		/* Internet Header Length */
	unsigned	ver		: 4;		/* Version */
	u8			tos;				/* Type Of Service */
	u16			tl;					/* Total Length */
	u16			id;					/* Identification */
	u16			f;					/* Flags & Fragment Offset */
	u8			ttl;				/* Time to Live */
	u8			proto;				/* Protocol on top of IP */
	u16			cs;					/* Header Checksum */
} ip_hdr_t;

typedef struct __attribute__ (( packed ))
{
	u8			sa[16];				/* Source Address */
	u8			da[16];				/* Destination Address */
	u8			payload[0];
} ip_ipv6_body_t;

typedef struct __attribute__ (( packed ))
{
	u8			sa[4];				/* Source Address */
	u8			da[4];				/* Destination Address */
	u8			payload[0];
} ip_ipv4_body_t;

typedef struct __attribute__ (( packed ))
{
	ip_hdr_t	hdr;
	u8			payload[0];
} ip_pkt_t;

#endif
