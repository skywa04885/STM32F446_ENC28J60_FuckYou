#ifndef _SRC_INTERNET_MICROCHIP_ENC28J60_H
#define _SRC_INTERNET_MICROCHIP_ENC28J60_H

/*********************************************
 * C Standard Library
 *********************************************/

#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/*********************************************
 * Project Headers
 *********************************************/

#include "../../hardware/spi.h"
#include "../../hardware/gpio.h"
#include "../../hardware/rcc.h"
#include "../../hardware/timers.h"
#include "../../types.h"

#include "../checksum.h"
#include "../bswap.h"
#include "../manager.h"

#include "enc28j60_registers.h"

#include "../ethernet.h"
#include "../arp.h"
#include "../udp.h"
#include "../ip.h"
#include "../icmp.h"

/*********************************************
 * Data Types
 *********************************************/

typedef enum
{
	ENC28J60_SPI_CMD_RCR = 0,
	ENC28J60_SPI_CMD_RBM,
	ENC28J60_SPI_CMD_WCR,
	ENC28J60_SPI_CMD_WBM,
	ENC28J60_SPI_CMD_BFS,
	ENC28J60_SPI_CMD_BFC,
	ENC28J60_SPI_CMD_SRC = 7
} enc28j60_spi_command_t;

typedef enum
{
	ENC28J60_BANK_0 = 0,
	ENC28J60_BANK_1,
	ENC28J60_BANK_2,
	ENC28J60_BANK_3
} enc28j60_bank_t;

typedef struct __attribute__ (( packed ))
{
	u16 rbc;
	u16 flags;
} enc28j60_status_vector_t;

typedef struct __attribute__ (( packed ))
{
	u8 cb;
	ethernet_pkt_t eth_pkt;
} enc28j60_pkt_t;

/*********************************************
 * ENC28J60 SPI
 *********************************************/

void 		enc28j60_spi_init(void);
void		enc28j60_spi_select(void);
void		enc28j60_spi_deselect(void);
u8 			enc28j60_spi_transceive(u8 byte);

/*********************************************
 * ENC28J60 Communication
 *********************************************/

void		enc28j60_wbm(const u8 *data, u16 len);
void		enc28j60_rbm(u8 *data, u16 len);

void		enc28j60_wcr(u8 reg, u8 val);

void		enc28j60_bfs(u8 reg, u8 mask);
void		enc28j60_bfc(u8 reg, u8 mask);

u8			enc28j60_eth_rcr(u8 reg);
u8			enc28j60_mac_rcr(u8 reg);

void		enc28j60_eth_wait_until_set(u8 reg, u8 bit);
void		enc28j60_mac_wait_until_set(u8 reg, u8 bit);
void 		enc28j60_eth_wait_until_clear(u8 reg, u8 bit);
void 		enc28j60_mac_wait_until_clear(u8 reg, u8 bit);

void		enc28j60_src(void);

void		enc28j60_set_wbm_address(u16 address);
void		enc28j60_set_rbm_address(u16 address);

/*********************************************
 * ENC28J60 Default Functions
 *********************************************/

void		enc28j60_phy_write(u8 reg, u16 val);
u16			enc28j60_phy_read(u8 reg);

void		enc28j60_bank_select(enc28j60_bank_t bank);

void		enc28j60_write_mac(u8 *mac);
void		enc28j60_mac_read(u8 *mac);

void		enc28j60_leda_mode(enc28j60_phlcon_lcfg_t mode);
void		enc28j60_ledb_mode(enc28j60_phlcon_lcfg_t mode);

void		enc28j60_rx_enable(void);
void		enc28j60_rx_disable(void);

bool		enc28j60_is_link_up(void);

/*********************************************
 * ENC28J60 Initialization
 *********************************************/

void 		enc28j60_init(void);
void 		enc28j60_phy_init(void);
void 		enc28j60_filter_init(void);
void		enc28j60_rx_init(void);
void 		enc28j60_mac_init(void);

/*********************************************
 * ENC28J60 Networking
 *********************************************/

u8			enc28j60_packet_count(void);
void		enc28j60_pkt_prepare(enc28j60_pkt_t *pkt, uint8_t *da, u16 type);
void		enc28j60_write(const enc28j60_pkt_t *pkt, u16 len);
bool		enc28j60_read(enc28j60_pkt_t *pkt);
void 		enc28j60_poll(u8 *buffer);

/*********************************************
 * ENC28J60 Networking ( ARP )
 *********************************************/

void 		enc28j60_handle_arp(enc28j60_pkt_t *pkt);
void 		enc28j60_handle_arp_reply(enc28j60_pkt_t *pkt);
void 		enc28j60_handle_arp_request(enc28j60_pkt_t *pkt);

/*********************************************
 * ENC28J60 Networking ( IP )
 *********************************************/

void		enc28j60_ipv4_prepare(ip_pkt_t *ip_pkt, u8 *ipv4);
void 		enc28j60_ipv4_finish(ip_pkt_t *ip_pkt);
void		enc28j60_handle_ipv4(enc28j60_pkt_t *pkt);

/*********************************************
 * ENC28J60 Networking ( ICMP )
 *********************************************/

void		enc28j60_handle_icmp_ipv4(enc28j60_pkt_t *pkt);

/*********************************************
 * ENC28J60 Networking ( UDP )
 *********************************************/

void 		enc28j60_ipv4_udp_prepare(ip_pkt_t *ip_pkt, udp_pkt_t *udp_pkt, u16 port);
void		enc28j60_handle_ipv4_udp(enc28j60_pkt_t *pkt);

#endif
