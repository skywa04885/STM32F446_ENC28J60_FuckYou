#ifndef _SRC_INTERNET_CUSTOM_H
#define _SRC_INTERNET_CUSTOM_H

/*********************************************
 * Project Headers
 *********************************************/

#include "../../types.h"

/*********************************************
 * Data Types
 *********************************************/

#define DISCOVER_PKT_PORT					7512

typedef enum
{
	DISCOVER_OPCODE_REQUEST = 0,
	DISCOVER_OPCODE_RESPONSE = 1
} discover_pkt_opcode_t;

typedef struct __attribute__ (( packed ))
{
	u8 		p[3];			/* Prefix, used to make sure it is a discover packet */
	u8 		op;				/* Opcode, tells us the type of packet */
	u8		vstrl;			/* Vendor-String Length */
	char	vstr[0];		/* Vendor-String, tells the sender about us */
} discover_pkt_t;

/*********************************************
 * Prototypes
 *********************************************/

u8 *discover_pkt_prefix();

#endif
