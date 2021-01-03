#ifndef _SRC_INTERNET_DNS_H
#define _SRC_INTERNET_DNS_H

/*********************************************
 * C Standard Library
 *********************************************/

#include <string.h>

/*********************************************
 * Project Headers
 *********************************************/

#include "../types.h"

/*********************************************
 * Data Types
 *********************************************/

#define DNS_PORT							53

typedef enum
{
	DNS_RR_A = 1,
	DNS_RR_NS,
	DNS_RR_MD,
	DNS_RR_MF,
	DNS_RR_CNAME,
	DNS_RR_SOA,
	DNS_RR_MB,
	DNS_RR_MG,
	DNS_RR_MR,
	DNS_RR_NULL,
	DNS_RR_WKS,
	DNS_RR_PTR,
	DNS_RR_HINFO,
	DNS_RR_MINFO,
	DNS_RR_MX,
	DNS_RR_TXT
} dns_rr_type_t;

typedef enum
{
	DNS_QTYPE_AXFR = 252,
	DNS_RR_MAILB,
	DNS_RR_MAILA,
	DNS_RR_ALL
} dns_qtype_t;

typedef enum
{
	DNS_CLASS_IN = 1,
	DNS_CLASS_CS,
	DNS_CLASS_CH,
	DNS_CLASS_HS
} dns_class_t;

typedef enum
{
	DNS_QCLASS_ANY = 255
} dns_qclass_t;

typedef enum
{
	DNS_OPCODE_QUERY = 0,
	DNS_OPCODE_IQUERY,
	DNS_OPCODE_STATUS
} dns_opcode_t;

typedef enum
{
	DNS_RCODE_NO_ERROR = 0,
	DNS_RCODE_FORMAT,
	DNS_RCODE_SERVER_FAILURE,
	DNS_RCODE_NAME_ERROR,
	DNS_RCODE_NOT_IMPLEMENTED,
	DNS_RCODE_REFUSED
} dns_rcode_t;

typedef struct __attribute__ (( packed ))
{
	u16			id;				/* A 16 bit identifier assigned by the program that generates any kind of query. */
	unsigned	qr		: 1;	/* A one bit field that specifies whether this message is a query (0), or a response (1). */
	unsigned	opcode	: 4;	/* A four bit field that specifies kind of query in this message. */
	unsigned	aa		: 1;	/* Authoritative Answer */
	unsigned	tc		: 1;	/* TrunCation */
	unsigned	rd		: 1;	/* Recursion Desired */
	unsigned	ra		: 1;	/* Recursion Available */
	unsigned	z		: 3;	/* Reserved, Zero */
	unsigned	rcode	: 4;	/* Response Code */
	u16			qdcnt;			/* an unsigned 16 bit integer specifying the number of entries in the question section. */
	u16			ancnt;			/* an unsigned 16 bit integer specifying the number of resource records in the answer section. */
	u16			nscnt;			/* an unsigned 16 bit integer specifying the number of name server resource records in the authority records section. */
	u16			arcnt;			/* an unsigned 16 bit integer specifying the number of resource records in the additional records section. */
} dns_hdr_t;

typedef struct __attribute__ (( packed ))
{
	dns_hdr_t	hdr;
	u8			payload[0];
} dns_pkt_t;

typedef struct __attribute__ (( packed ))
{
	u16 	qtype;
	u16 	qclass;
	u8		next[0];
} dns_question_t;

typedef struct __attribute__ (( packed ))
{
	u8 		len;
	u8		next[0];
} dns_label_seg_t;

/*********************************************
 * Prototypes
 *********************************************/

u8 *dns_add_question(u8 *p, const char *qname, dns_qtype_t qtype, dns_qclass_t qclass);

dns_label_seg_t *dns_label_parser_next(dns_label_seg_t *p);

#endif
