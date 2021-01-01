#ifndef _SRC_INTERNET_UDP_H
#define _SRC_INTERNET_UDP_H

/*********************************************
 * Project Headers
 *********************************************/

#include "../types.h"

#include "checksum.h"
#include "bswap.h"

/*********************************************
 * Data Types
 *********************************************/

typedef struct __attribute__ (( packed ))
{
	u16		sp;			/* Source Port */
	u16		dp;			/* Destination Port */
	u16		l;			/* Length */
	u16		cs;			/* Checksum */
} udp_hdr_t;

typedef struct __attribute__ (( packed ))
{
	udp_hdr_t	hdr;
	u8			payload[0];
} udp_pkt_t;

/*********************************************
 * Prototypes
 *********************************************/

u16		udp_calc_cs(const udp_pkt_t *udp_pkt, const u8 *da, const u8 *sa, u8 sal, u8 dal, u8 proto);

#endif
