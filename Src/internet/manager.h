#ifndef _SRC_INTERNET_MANAGER_H
#define _SRC_INTERNET_MANAGER_H

/*********************************************
 * C Standard Library
 *********************************************/

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/*********************************************
 * Project Headers
 *********************************************/

#include "../types.h"
#include "../logger.h"
#include "bootp.h"
#include "packet-builder.h"

#include "microchip/enc28j60.h"


/*********************************************
 * Macros
 *********************************************/

#define MANAGER_MAX_TCP_SOCKETS			4
#define MANAGER_MAX_UDP_SOCKETS			4

/*********************************************
 * Data Types
 *********************************************/

typedef enum
{
	MANAGER_DHCP_STATE_NOT_READY,
	MANAGER_DHCP_STATE_DISCOVER,
	MANAGER_DHCP_STATE_REQUEST,
	MANAGER_DHCP_STATE_READY
} manager_dhcp_state_t;

typedef struct
{
	/* State */
	manager_dhcp_state_t dhcp_state;
	/* Hardware Configuration */
	u16 max_frame_length;
	u16 tx_buff_start, rx_buff_end;
	/* Configuration */
	u8 mac[6];
	u8 ipv4_address[4], ipv4_subnet_mask[4];
	/* Servers */
	u8 ipv4_dns_server[4], ipv4_dhcp_server[4], ipv4_router[4];
	/* Options */
	unsigned full_duplex : 1;
	unsigned ready : 1;
} manager_config_t;

/*********************************************
 * Prototypes
 *********************************************/

void manager_init(void);
void manager_dhcp_init(void);
void manager_poll(void);

#endif
