#ifndef _SRC_INTERNET_ICMP_H
#define _SRC_INTERNET_ICMP_H

/*********************************************
 * Project Headers
 *********************************************/

#include "../types.h"
#include "checksum.h"

/*********************************************
 * Data Types
 *********************************************/

typedef enum {
	ICMP_TYPE_ECHO_REPLY = 0,
	ICMP_TYPE_DESTINATION_UNREACHABLE = 3,
	ICMP_TYPE_SOURCE_QUENCH,
	ICMP_TYPE_REDIRECT,
	ICMP_TYPE_ECHO = 8,
	ICMP_TYPE_TIME_EXCEEDED = 11,
	ICMP_TYPE_PARAMETER_PROBLEM,
	ICMP_TYPE_TIMESTAMP,
	ICMP_TYPE_TIMESTAMP_REPLY,
	ICMP_TYPE_INFORMATION_REQUEST,
	ICMP_TYPE_INFORMATION_REPLY
} icmp_type_t;

typedef struct __attribute__ (( packed ))
{
	u8	type;	/* ICMP Type */
	u8	code;	/* ICMP Code */
	u16	cs;		/* ICMP Checksum */
} icmp_hdr_t;

typedef struct __attribute__ (( packed ))
{
	icmp_hdr_t	hdr;
	u8			payload[0];
} icmp_pkt_t;

/* Destination Unreachable Message */

#define ICMP_DUM_CODE_NET_UNREACH	0		/* net unreachable; */
#define ICMP_DUM_CODE_HOST_UNREACH	1		/* host unreachable; */
#define ICMP_DUM_CODE_PROTO_UNREACH	2		/* protocol unreachable; */
#define ICMP_DUM_CODE_PORT_UNREACH	3		/* port unreachable; */
#define ICMP_DUM_CODE_FRAG_ND_DF_SET	4		/* fragmentation needed and DF set; */
#define ICMP_DUM_CODE_SRC_ROUTE_FAIL	5		/* source route failed. */

typedef struct __attribute__ (( packed ))
{
	u32 unused;		/* 4 Bytes of zeroes */
	u8	body[0];	/* Internet Header + 64 bit of data */
} icmp_dum_t;

/* Time Exceeded Message */

#define ICMP_PPM_CODE_TTL_EXC_INTR	0		/* time to live exceeded in transit; */
#define ICMP_PPM_CODE_FRAG_REA_EXC	1		/* fragment reassemble time exceeded. */

typedef struct __attribute__ (( packed ))
{
	u32 unused;		/* 4 Bytes of zeroes */
	u8	body[0];	/* Internet Header + 64 bit of data */
} icmp_tem_t;

/* Parameter Problem Message */

#define ICMP_PPM_CODE				0

typedef struct __attribute__ (( packed ))
{
	u8	pointer;	/* Byte of error */
	u8	unused[3];	/* 3 Bytes of zeroes */
	u8	body[0];	/* Internet Header + 64 bit of data */
} icmp_ppm_t;

/* Source Quench Message */

#define ICMP_SQM_CODE				0		/* pointer indicates the error. */

typedef struct __attribute__ (( packed ))
{
	u32 unused;		/* 4 Bytes of zeroes */
	u8	body[0];	/* Internet Header + 64 bit of data */
} icmp_sqm_t;

/* Redirect Message */

#define ICMP_RM_CODE_REDIR_DGR_NET	0		/* Redirect datagrams for the Network. */
#define ICMP_RM_CODE_REDIR_DGR_HOST	1		/* Redirect datagrams for the Host. */
#define ICMP_RM_CODE_REDIR_DGR_TOSAN	2		/* Redirect datagrams for the Type of Service and Network. */
#define ICMP_RM_CODE_REDIR_DGR_TOSAH	3		/* Redirect datagrams for the Type of Service and Host. */

typedef struct __attribute__ (( packed ))
{
	u8	gia[4];		/* Gateway Internet Address */
	u8	body[0];	/* Internet Header + 64 bit of data */
} icmp_rm_t;

/* Echo or Echo Reply Message */

#define ICMP_ERM_CODE				0

typedef struct __attribute__ (( packed ))
{
	u16	id;			/* Identifier */
	u16	sn;			/* Sequence Number */
	u8	data[0];	/* Data of packet */
} icmp_erm_t;

/*  Timestamp or Timestamp Reply Message */

#define ICMP_TTR_CODE				0

typedef struct __attribute__ (( packed ))
{
	u16	id;			/* Identifier */
	u16	sn;			/* Sequence Number */
	u32	ot;			/* Originate Timestamp */
	u32	rt;			/* Receive Timestamp */
	u32	tt;			/* Transmit Timestamp  */
} icmp_ttr_t;

/* Information Request or Information Reply Message */
#define ICMP_IRR_CODE				0

typedef struct __attribute__ (( packed ))
{
	u16	id;			/* Identifier */
	u16	sn;			/* Sequence Number */
} icmp_irr_t;

/*********************************************
 * Prototypes
 *********************************************/

u16 icmp_calc_cs(const icmp_pkt_t *pkt, u16 len);

#endif
