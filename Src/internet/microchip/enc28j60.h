#ifndef _SRC_INTERNET_MICROCHIP_ENC28J60_H
#define _SRC_INTERNET_MICROCHIP_ENC28J60_H

/*********************************************
 * C Standard Library
 *********************************************/

#include <string.h>

/*********************************************
 * Project Headers
 *********************************************/

#include "../../hardware/spi.h"
#include "../../hardware/gpio.h"
#include "../../hardware/rcc.h"
#include "../ethernet.h"
#include "../../types.h"
#include "../bswap.h"

#include "enc28j60_registers.h"

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

typedef struct
{
	/* Hardware Configuration */
	u16 max_frame_length;
	u16 tx_buff_start, rx_buff_end;
	/* Addresses */
	u8 mac[6];
	u8 ipv4[4];
	/* Options */
	unsigned full_duplex : 1;
} enc28j60_config_t;

typedef struct __attribute__ (( packed ))
{
	u8 cb;
	ethernet_pkt_t eth_pkt;
} enc28j60_pkt_t;

/*********************************************
 * ENC28J60 Static Functions
 *********************************************/

enc28j60_config_t *enc28j60_get_config();

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

void		enc28j60_pkt_prepare(enc28j60_pkt_t *pkt, uint8_t *da, u16 type);
void		enc28j60_write(const enc28j60_pkt_t *pkt, u16 len);
void		enc28j60_read(enc28j60_pkt_t *pkt);

#endif
