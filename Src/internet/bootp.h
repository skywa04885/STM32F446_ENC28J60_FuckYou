#ifndef _SRC_INTERNET_BOOTP_H
#define _SRC_INTERNET_BOOTP_H

/*********************************************
 * C Standard Library
 *********************************************/

#include <stdbool.h>
#include <string.h>

/*********************************************
 * Project Headers
 *********************************************/

#include "../types.h"

/*********************************************
 * Data Types
 *********************************************/

#define BOOTP_UDP_SERVER_PORT				67
#define BOOTP_UDP_CLIENT_PORT				68

#define BOOTP_OPCODE_BOOTREQUEST				1
#define BOOTP_OPCODE_BOOTREPLY				2

#define BOOTP_FLAG_BROADCAST					15

typedef enum
{
	DHCP_MESSAGE_TYPE_DHCPDISCOVER = 1,
	DHCP_MESSAGE_TYPE_DHCPOFFER = 2,
	DHCP_MESSAGE_TYPE_DHCPREQUEST = 3,
	DHCP_MESSAGE_TYPE_DHCPDECLINE = 4,
	DHCP_MESSAGE_TYPE_DHCPACK = 5,
	DHCP_MESSAGE_TYPE_DHCPNAK = 6,
	DHCP_MESSAGE_TYPE_DHCPRELEASE = 7
} dhcp_message_type_t;

typedef enum
{
	BOOTP_OPTION_CODE_PAD = 0,
	/* Vendor Extensions */
	BOOTP_OPTION_CODE_SUBNET_MASK = 1,
	BOOTP_OPTION_CODE_TIME_OFFSET = 2,
	BOOTP_OPTION_CODE_ROUTER_OPTION = 3,
	BOOTP_OPTION_CODE_TIME_SERVER = 4,
	BOOTP_OPTION_CODE_NAME_SERVER = 5,
	BOOTP_OPTION_CODE_DNS_SERVER = 6,
	BOOTP_OPTION_CODE_LOG_SERVER = 7,
	BOOTP_OPTION_CODE_COOKIE_SERVER = 8,
	BOOTP_OPTION_CODE_LPR_SERVER = 9,
	BOOTP_OPTION_CODE_IMPRESS_SERVER = 10,
	BOOTP_OPTION_CODE_RESOURCE_LOCATION_SERVER = 11,
	BOOTP_OPTION_CODE_HOSTNAME = 12,
	BOOTP_OPTION_CODE_DOMAIN_NAME = 15,
	/* IP Layer Parameters per host */
	BOOTP_OPTION_CODE_MAX_DATAGRAM_REASSEMBLY_SIZE = 22,
	BOOTP_OPTION_CODE_DEFAULT_IP_TTL = 23,
	/* IP Layer Parameters */
	BOOTP_OPTION_CODE_ALL_SUBNETS_ARE_LOCAL = 27,
	BOOTP_OPTION_CODE_BROADCAST_ADDR = 28,
	BOOTP_OPTION_CODE_PERFORM_MASK_DISCOVERY = 29,
	BOOTP_OPTION_CODE_MASK_SUPPLIER = 30,
	BOOTP_OPTION_CODE_PERFORM_ROUTER_DISCOVERY = 31,
	BOOTP_OPTION_CODE_ROUTER_SOLICITATION_ADDRESS = 32,
	/* Link Layer Parameters */
	BOOTP_OPTION_CODE_TRAILER_ENCAPSULATION = 34,
	BOOTP_OPTION_CODE_ARP_CACHE_TIMEOUT = 35,
	BOOTP_OPTION_CODE_ETHERNET_ENCAPSULATION = 36,
	/* TCP Parameters */
	BOOTP_OPTION_CODE_TCP_DEFAULT_TTL = 37,
	BOOTP_OPTION_CODE_TCP_KEEPALIVE_INTERVAL = 38,
	BOOTP_OPTION_CODE_TCP_KEEPALIVE_GARBAGE = 39,
	/* Application and Service Parameters */
	BOOTP_OPTION_CODE_NETWORK_INFO_SERVICE_DOMAIN = 40,
	BOOTP_OPTION_CODE_NETWORK_INFO_SERVERS = 41,
	BOOTP_OPTION_CODE_NTP_SERVERS = 42,
	BOOTP_OPTION_CODE_VENDOR_SPECIFIC_INFO = 43,
	/* DHCP Extensions */
	BOOTP_OPTION_CODE_DHCP_REQUESTED_IP_ADDR = 50,
	BOOTP_OPTION_CODE_DHCP_IP_ADDR_RELEASE_TIME = 51,
	BOOTP_OPTION_CODE_DHCP_OPTION_OVERLOAD = 52,
	BOOTP_OPTION_CODE_DHCP_MESSAGE_TYPE = 53,
	BOOTP_OPTION_CODE_DHCP_SERVER_ID = 54,
	BOOTP_OPTION_CODE_DHCP_PARAMETER_REQUEST_LIST = 55,
	BOOTP_OPTION_CODE_DHCP_MESSAGE = 56,
	BOOTP_OPTION_CODE_DHCP_MAX_MESSAGE_SIZE = 57,
	BOOTP_OPTION_CODE_DHCP_RENEW_TIME = 58,
	BOOTP_OPTION_CODE_DHCP_REBINDING_TIME = 59,
	BOOTP_OPTION_CODE_DHCP_CLASS_ID = 60,
	BOOTP_OPTION_CODE_DHCP_CLIENT_ID = 61,
	/* ----------------- */
	BOOTP_OPTION_CODE_END = 255
} dhcp_oparam_code_t;

/* Optional Parameters  */

typedef struct __attribute__ (( packed ))
{
	u8		code;
	u8		len;
	u8		body[0];
} bootp_oparam_t;

typedef struct __attribute__ (( packed ))
{
	u8 		addr[4];
	u8		next[0];
} bootp_oparam_addr_t;

typedef struct __attribute__ (( packed ))
{
	bool	val;
	u8		next[0];
} bootp_oparam_bool_t;

typedef struct __attribute__ (( packed ))
{
	u8		val;
	u8		next[0];
} bootp_oparam_u8_t;

typedef struct __attribute__ (( packed ))
{
	u16		val;
	u8		next[0];
} bootp_oparam_u16_t;

typedef struct __attribute__ (( packed ))
{
	u32		val;
	u8		next[0];
} bootp_oparam_u32_t;

typedef struct __attribute__ (( packed ))
{
	u64		val;
	u8		next[0];
} bootp_oparam_u64_t;

typedef struct __attribute__ (( packed ))
{
	char	string[0];
} bootp_oparam_string_t;

typedef struct __attribute__ (( packed ))
{
	u8		values[4];
	u8 		next[0];
} bootp_oparam_dhcp_cookie_t;

#define BOOTP_OPARAM_U8(A) ((bootp_oparam_u8_t *) (A))
#define BOOTP_OPARAM_U8_NEXT(A) (((bootp_oparam_u8_t *) (A))->next)

#define BOOTP_OPARAM_ADDR(A) ((bootp_oparam_addr_t *) (A))
#define BOOTP_OPARAM_ADDR_NEXT(A) (((bootp_oparam_addr_t *) (A))->next)

/* Packets */

typedef struct __attribute__ (( packed ))
{
	u8 		op;			/* Message OPcode / message type. 1 = BOOTREQUEST, 2 = BOOTREPLY */
	u8 		htype;		/* Hardware address type, see ARP section in "Assigned Numbers" RFC; e.g., '1' = 10mb Ethernet. */
	u8 		hlen;		/* Hardware address length (e.g.  '6' for 10mb Ethernet). */
	u8 		hops;		/* Client sets to zero, optionally used by relay agents when booting via a relay agent. */
	u32 	xid;		/* Transaction ID, a random number chosen by the client, used by the client and server to associate messages and responses between a client and a server. */
	u16 	secs;		/* Filled in by client, seconds elapsed since client began address acquisition or renewal process. */
	u16 	flags;		/* Just the flags */
} bootp_hdr_t;

typedef struct __attribute__ (( packed ))
{
	u8		ciaddr[4];	/* Client IP address; only filled in if client is in BOUND, RENEW or REBINDING state and can respond to ARP requests. */
	u8		yiaddr[4];	/* 'your' (client) IP address. */
	u8		siaddr[4];	/* IP address of next server to use in bootstrap; returned in DHCPOFFER, DHCPACK by server.  */
	u8		giaddr[4];	/* Relay agent IP address, used in booting via a relay agent. */
	u8		chaddr[16];	/* Client hardware address. */
	char	sname[64];	/* Optional server host name, null terminated string. */
	u8		file[128];	/* Boot file name, null terminated string; "generic" name or null in DHCPDISCOVER, fully qualified directory-path name in DHCPOFFER. */
	u8		payload[0];	/* Optional parameters field. See the options documents for a list of defined options. */
} bootp_body_t;

typedef struct __attribute__ (( packed ))
{
	bootp_hdr_t 	hdr;
	bootp_body_t	body;
} bootp_pkt_t;

/*********************************************
 * Prototypes
 *********************************************/

bootp_oparam_t *bootp_init_dhcp_options(bootp_pkt_t *bootp_pkt);

bootp_oparam_t *bootp_oparam_add_u8(u8 code, u8 val, bootp_oparam_t *param);
bootp_oparam_t *bootp_oparam_add_addr(u8 code, u8 *addr, bootp_oparam_t *param);
bootp_oparam_t *bootp_oparam_end(bootp_oparam_t *param);

bootp_oparam_t *bootp_oparam_parser_init_dhcp(bootp_pkt_t *bootp_pkt);
bootp_oparam_t *bootp_oparam_parser_next(bootp_oparam_t *param);

#endif
