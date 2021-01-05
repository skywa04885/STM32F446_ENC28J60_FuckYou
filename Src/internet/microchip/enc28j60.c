#include "enc28j60.h"

extern void udp_packet_callback(enc28j60_pkt_t *pkt);

/*********************************************
 * ENC28J60 Static Variables
 *********************************************/

extern manager_config_t config;

/*********************************************
 * ENC28J60 SPI
 *********************************************/

void enc28j60_spi_init(void)
{
	// Enables the GPIOA Clock, and SPI1 Clock
	*RCC_AHB1ENR |= (_BV(RCC_AHB1ENR_GPIOAEN));
	*RCC_APB2ENR |= (_BV(RCC_APB2ENR_SPI1EN));

	// Configures GPIOA ( MISO, MOSI, SCK, CS )
	*GPIO_MODER(GPIOA_BASE) &= ~(GPIO_MODE(7, GPIO_MODE_RESET) |
			GPIO_MODE(6, GPIO_MODE_RESET) |
			GPIO_MODE(5, GPIO_MODE_RESET) |
			GPIO_MODE(8, GPIO_MODE_RESET));

	*GPIO_MODER(GPIOA_BASE) |= (GPIO_MODE(7, GPIO_MODE_AF) |
			GPIO_MODE(6, GPIO_MODE_AF) |
			GPIO_MODE(5, GPIO_MODE_AF) |
			GPIO_MODE(8, GPIO_MODE_OUTPUT));

	*GPIO_AFRL(GPIOA_BASE) |= (GPIO_AF(7, 5) | GPIO_AF(6, 5) |
			GPIO_AF(5, 5));

	*GPIO_OSPEEDR(GPIOA_BASE) |= (GPIO_OSPEED(8, GPIO_HIGH_SPEED));

	// Configures the SPI Hardware
	*SPI_CR1(SPI1_BASE) = (SPI_CR1_BR(SPI_CR1_BR_DIV4) | _BV(SPI_CR1_MSTR));
	*SPI_CR2(SPI1_BASE) = (_BV(SPI_CR2_SSOE));
	*SPI_CR1(SPI1_BASE) |= (_BV(SPI_CR1_SPE));

}

void enc28j60_spi_select(void)
{
	for (u8 i = 0; i < 5; ++i) asm("NOP\n");
	*GPIO_ODR(GPIOA_BASE) &= ~(_BV(8));
}

void enc28j60_spi_deselect(void)
{
	for (u8 i = 0; i < 5; ++i) asm("NOP\n");
	*GPIO_ODR(GPIOA_BASE) |= (_BV(8));
}

u8 enc28j60_spi_transceive(u8 byte)
{
	while (!(*SPI_SR(SPI1_BASE) & _BV(SPI_SR_TXE)));
	*SPI_DR(SPI1_BASE) = byte;
	while (!(*SPI_SR(SPI1_BASE) & _BV(SPI_SR_RXNE)));
	return *SPI_DR(SPI1_BASE);
}

/*********************************************
 * ENC28J60 Communication
 *********************************************/

void enc28j60_wbm(const u8 *data, u16 len)
{
	enc28j60_spi_select();

	enc28j60_spi_transceive((ENC28J60_SPI_CMD_WBM << 5) | 0x1A);
	for (u16 i = 0; i < len; ++i)
	{
		enc28j60_spi_transceive(data[i]);
	}

	enc28j60_spi_deselect();
}

void enc28j60_rbm(u8 *data, u16 len)
{
	enc28j60_spi_select();

	enc28j60_spi_transceive((ENC28J60_SPI_CMD_RBM << 5) | 0x1A);
	for (u16 i = 0; i < len; ++i)
	{
		data[i] = enc28j60_spi_transceive(0xFF);
	}

	enc28j60_spi_deselect();
}

void enc28j60_wcr(u8 reg, u8 val)
{
	enc28j60_spi_select();

	enc28j60_spi_transceive((ENC28J60_SPI_CMD_WCR << 5) | (reg & 0x1F));
	enc28j60_spi_transceive(val);

	enc28j60_spi_deselect();
}

void enc28j60_bfs(u8 reg, u8 mask)
{
	enc28j60_spi_select();

	enc28j60_spi_transceive((ENC28J60_SPI_CMD_BFS << 5) | (reg & 0x1F));
	enc28j60_spi_transceive(mask);

	enc28j60_spi_deselect();
}

void enc28j60_bfc(u8 reg, u8 mask)
{
	enc28j60_spi_select();

	enc28j60_spi_transceive((ENC28J60_SPI_CMD_BFC << 5) | (reg & 0x1F));
	enc28j60_spi_transceive(mask);

	enc28j60_spi_deselect();
}

u8 enc28j60_eth_rcr(u8 reg)
{
	register u8 val;

	enc28j60_spi_select();

	enc28j60_spi_transceive((ENC28J60_SPI_CMD_RCR << 5) | (reg & 0x1F));
	val = enc28j60_spi_transceive(0xFF);

	enc28j60_spi_deselect();

	return val;
}

u8 enc28j60_mac_rcr(u8 reg)
{
	register u8 val;

	enc28j60_spi_select();

	enc28j60_spi_transceive((ENC28J60_SPI_CMD_RCR << 5) | (reg & 0x1F));
	enc28j60_spi_transceive(0xFF); 			// Dummy Byte
	val = enc28j60_spi_transceive(0xFF);

	enc28j60_spi_deselect();

	return val;
}

void enc28j60_eth_wait_until_set(u8 reg, u8 bit)
{
	while (!(enc28j60_eth_rcr(reg) & _BV(bit)));
}

void enc28j60_mac_wait_until_set(u8 reg, u8 bit)
{
	while (!(enc28j60_mac_rcr(reg) & _BV(bit)));
}

void enc28j60_eth_wait_until_clear(u8 reg, u8 bit)
{
	while (enc28j60_eth_rcr(reg) & _BV(bit));
}

void enc28j60_mac_wait_until_clear(u8 reg, u8 bit)
{
	while (enc28j60_mac_rcr(reg) & _BV(bit));
}

void enc28j60_src(void)
{
	enc28j60_spi_select();
	enc28j60_spi_transceive((ENC28J60_SPI_CMD_SRC << 5) | 0x1F);
	enc28j60_spi_deselect();
}

void enc28j60_set_wbm_address(u16 address)
{
	enc28j60_bank_select(ENC28J60_BANK_0);
	enc28j60_wcr(ENC28J60_BK0_EWRPTL, (u8) address);
	enc28j60_wcr(ENC28J60_BK0_EWRPTH, (u8) (address >> 8));
}

void enc28j60_set_rbm_address(u16 address)
{
	enc28j60_bank_select(ENC28J60_BANK_0);
	enc28j60_wcr(ENC28J60_BK0_ERDPTL, (u8) address);
	enc28j60_wcr(ENC28J60_BK0_ERDPTH, (u8) (address >> 8));
}

/*********************************************
 * ENC28J60 Default Functions
 *********************************************/

void enc28j60_phy_write(u8 reg, u16 val)
{
	enc28j60_bank_select(ENC28J60_BANK_2);

	// 1. Write the address of the PHY register to write to
	//  into the MIREGADR register.

	enc28j60_wcr(ENC28J60_BK2_MIREGADR, reg);

	// 2. Write the lower 8 bits of data to write into the
	//  MIWRL register.

	enc28j60_wcr(ENC28J60_BK2_MIWRL, (u8) val);

	// 3. Write the upper 8 bits of data to write into the
	//  MIWRH register. Writing to this register auto-
	//  matically begins the MIIM transaction, so it must
	//  be written to after MIWRL. The MISTAT.BUSY
	//  bit becomes set.

	enc28j60_wcr(ENC28J60_BK2_MIWRH, (u8) (val >> 8));

	// 4. Waits until the BUSY flag in MISTAT is cleared
	enc28j60_bank_select(ENC28J60_BANK_3);
	enc28j60_mac_wait_until_clear(ENC28J60_BK3_MISTAT, ENC28J60_BK3_MISTAT_BUSY);
}

u16	enc28j60_phy_read(u8 reg)
{
	enc28j60_bank_select(ENC28J60_BANK_2);

	// 1. Write the address of the PHY register to read
	//  from into the MIREGADR register.

	enc28j60_wcr(ENC28J60_BK2_MIREGADR, reg);

	// 2. Set the MICMD.MIIRD bit. The read operation
	//  begins and the MISTAT.BUSY bit is set.

	enc28j60_wcr(ENC28J60_BK2_MICMD, enc28j60_mac_rcr(ENC28J60_BK2_MICMD) | _BV(ENC28J60_BK2_MICMD_MIIRD));

	// 3. Wait 10.24 μs. Poll the MISTAT.BUSY bit to be
	//  certain that the operation is complete. While
	//  busy, the host controller should not start any
	//  MIISCAN operations or write to the MIWRH
	//  register.
	//  When the MAC has obtained the register
	//  contents, the BUSY bit will clear itself.

	enc28j60_bank_select(ENC28J60_BANK_3);
	enc28j60_mac_wait_until_clear(ENC28J60_BK3_MISTAT, ENC28J60_BK3_MISTAT_BUSY);
	enc28j60_bank_select(ENC28J60_BANK_2);

	// 4. Clear the MICMD.MIIRD bit.

	enc28j60_wcr(ENC28J60_BK2_MICMD, enc28j60_mac_rcr(ENC28J60_BK2_MICMD) & ~(_BV(ENC28J60_BK2_MICMD_MIIRD)));

	// 5. Read the desired data from the MIRDL and
	//  MIRDH registers. The order that these bytes are
	//  accessed is unimportant.

	register u16 val = (u16) enc28j60_mac_rcr(ENC28J60_BK2_MIRDH);
	val <<= 8;
	val |= (u16) enc28j60_mac_rcr(ENC28J60_BK2_MIRDL);

	return val;
}

void enc28j60_bank_select(enc28j60_bank_t bank)
{
	register u8 reg = enc28j60_eth_rcr(ENC28J60_ECON1);
	reg &= ~(0x3);
	reg |= (bank & 0x3);
	enc28j60_wcr(ENC28J60_ECON1, reg);
}

static u8 enc28j60_mac_rw_order[6] = {
		ENC28J60_BK3_MAADR1,
		ENC28J60_BK3_MAADR2,
		ENC28J60_BK3_MAADR3,
		ENC28J60_BK3_MAADR4,
		ENC28J60_BK3_MAADR5,
		ENC28J60_BK3_MAADR6
};

void enc28j60_write_mac(u8 *mac)
{
	enc28j60_bank_select(ENC28J60_BANK_3);

	for (uint8_t i = 0; i < 6; ++i)
	{
		enc28j60_wcr(enc28j60_mac_rw_order[i], mac[i]);
	}
}

void enc28j60_mac_read(u8 *mac)
{
	enc28j60_bank_select(ENC28J60_BANK_3);

	for (uint8_t i = 0; i < 6; ++i)
	{
		mac[i] = enc28j60_mac_rcr(enc28j60_mac_rw_order[i]);
	}
}

void enc28j60_leda_mode(enc28j60_phlcon_lcfg_t mode)
{
	register u16 reg16 = enc28j60_phy_read(ENC28J60_PHLCON);
	reg16 &= ~(ENC28J60_PHLCON_LACFG(0b1111));
	reg16 |= (ENC28J60_PHLCON_LACFG(mode));
	enc28j60_phy_write(ENC28J60_PHLCON, reg16);
}

void enc28j60_ledb_mode(enc28j60_phlcon_lcfg_t mode)
{
	register u16 reg16 = enc28j60_phy_read(ENC28J60_PHLCON);
	reg16 &= ~(ENC28J60_PHLCON_LBCFG(0b1111));
	reg16 |= (ENC28J60_PHLCON_LBCFG(mode));
	enc28j60_phy_write(ENC28J60_PHLCON, reg16);
}

void enc28j60_rx_enable(void)
{
	enc28j60_bfs(ENC28J60_ECON1, (_BV(ENC28J60_ECON1_RXEN)));
}

void enc28j60_rx_disable(void)
{
	enc28j60_bfc(ENC28J60_ECON1, (_BV(ENC28J60_ECON1_RXEN)));
}

bool enc28j60_is_link_up(void)
{
	return (enc28j60_phy_read(ENC28J60_PHSTAT2) & (_BV(ENC28J60_PHSTAT2_LSTAT)));
}

/*********************************************
 * ENC28J60 Initialization
 *********************************************/

void enc28j60_init(void)
{
	// Performs full system reset
	enc28j60_src();

	// Waits for the system clock to be ready
	enc28j60_eth_wait_until_set(ENC28J60_ESTAT, ENC28J60_ESTAT_CLKRDY);

	// Disables RX
	enc28j60_rx_disable();

	// Performs the MicroChip specified initialization sequence
	enc28j60_rx_init();
	enc28j60_filter_init();
	enc28j60_mac_init();
	enc28j60_phy_init();

	// Enables RX
	enc28j60_rx_enable();
}

void enc28j60_phy_init(void)
{
	register u16 reg16;

	// For proper duplex operation, the PHCON1.PDPXMD
	//  bit must also match the value of the MACON3.FULDPX
	//  bit.

	if (config.full_duplex)
	{
		reg16 = enc28j60_phy_read(ENC28J60_PHCON1);
		reg16 |= (_BV(ENC28J60_PHCON1_PDPXMD));
		enc28j60_phy_write(ENC28J60_PHCON1, reg16);
	}

	// If using half duplex, the host controller may wish to set
	//  the PHCON2.HDLDIS bit to prevent automatic
	//  loopback of the data which is transmitted.

	if (!config.full_duplex)
	{
		reg16 = enc28j60_phy_read(ENC28J60_PHCON2);
		reg16 |= (_BV(ENC28J60_PHCON2_HDLDIS));
		enc28j60_phy_write(ENC28J60_PHCON2, reg16);
	}
}

void enc28j60_filter_init(void)
{

}

void enc28j60_rx_init(void)
{
	enc28j60_bank_select(ENC28J60_BANK_0);

	// Sets the RX Buffer start
	enc28j60_wcr(ENC28J60_BK0_ERXSTL, 0x00);
	enc28j60_wcr(ENC28J60_BK0_ERXSTH, 0x00);

	// Sets the RX Buffer end
	enc28j60_wcr(ENC28J60_BK0_ERXNDL, (u8) config.rx_buff_end);
	enc28j60_wcr(ENC28J60_BK0_ERXNDH, (u8) (config.rx_buff_end >> 8));

	// Sets the initial hardware write position
	enc28j60_wcr(ENC28J60_BK0_ERXRDPTH, 0x00);
	enc28j60_wcr(ENC28J60_BK0_ERXRDPTL, 0x00);
}

void enc28j60_mac_init(void)
{
	register u8 reg8;

	enc28j60_bank_select(ENC28J60_BANK_2);

	// 1. Set the MARXEN bit in MACON1 to enable the
	//  MAC to receive frames. If using full duplex, most
	//  applications should also set TXPAUS and
	//  RXPAUS to allow IEEE defined flow control to
	//  function.

	reg8 = enc28j60_mac_rcr(ENC28J60_BK2_MACON1);
	reg8 |= (_BV(ENC28J60_BK2_MACON1_MARXEN));

	if (config.full_duplex)
	{
		reg8 |= (_BV(ENC28J60_BK2_MACON1_TXPAUS) | _BV(ENC28J60_BK2_MACON1_RXPAUS));
	}

	enc28j60_wcr(ENC28J60_BK2_MACON1, reg8);

	// 2. Configure the PADCFG, TXCRCEN and
	//  FULDPX bits of MACON3. Most applications
	//  should enable automatic padding to at least
	//  60 bytes and always append a valid CRC. For
	//  convenience, many applications may wish to set
	//  the FRMLNEN bit as well to enable frame length
	//  status reporting. The FULDPX bit should be set
	//  if the application will be connected to a
	//  full-duplex configured remote node; otherwise, it
	//  should be left clear.

	reg8 = enc28j60_mac_rcr(ENC28J60_BK2_MACON3);

	reg8 |= (ENC28J60_BK2_MACON3_PADCFG(0x1)
			| _BV(ENC28J60_BK2_MACON3_FRMLNEN)
			| _BV(ENC28J60_BK2_MACON3_TXCRCEN));

	if (config.full_duplex)
	{
		reg8 |= (_BV(ENC28J60_BK2_MACON3_FULDPX));
	}

	enc28j60_wcr(ENC28J60_BK2_MACON3, reg8);

	// 3. Configure the bits in MACON4. For conform-
	//  ance to the IEEE 802.3 standard, set the
	//  DEFER bit.

	if (!config.full_duplex)
	{
		reg8 = enc28j60_mac_rcr(ENC28J60_BK2_MACON4);
		reg8 |= (_BV(ENC28J60_BK2_MACON4_DEFER));
		enc28j60_wcr(ENC28J60_BK2_MACON4, reg8);
	}

	// 4. Program the MAMXFL registers with the maxi-
	//  mum frame length to be permitted to be received
	//  or transmitted. Normal network nodes are
	//  designed to handle packets that are 1518 bytes
	//  or less.

	enc28j60_wcr(ENC28J60_BK2_MAMXFLL, (u8) config.max_frame_length);
	enc28j60_wcr(ENC28J60_BK2_MAMXFLH, (u8) (config.max_frame_length >> 8));

	// 5. Configure the Back-to-Back Inter-Packet Gap
	//  register, MABBIPG. Most applications will pro-
	//  gram this register with 15h when Full-Duplex
	//  mode is used and 12h when Half-Duplex mode
	//  is used.

	if (config.full_duplex) enc28j60_wcr(ENC28J60_BK2_MABBIPG, 0x15);
	else enc28j60_wcr(ENC28J60_BK2_MABBIPG, 0x12);

	// 6. Configure the Non-Back-to-Back Inter-Packet
	//  Gap register low byte, MAIPGL. Most applications
	//  will program this register with 12h.

	enc28j60_wcr(ENC28J60_BK2_MAIPGL, 0x12);

	// 7. If half duplex is used, the Non-Back-to-Back
	//  Inter-Packet Gap register high byte, MAIPGH,
	//  should be programmed. Most applications will
	//  program this register to 0Ch.

	if (!config.full_duplex) enc28j60_wcr(ENC28J60_BK2_MAIPGH, 0x0C);

	// 8. If Half-Duplex mode is used, program the
	//  Retransmission and Collision Window registers,
	//  MACLCON1 and MACLCON2. Most applications
	//  will not need to change the default Reset values.
	//  If the network is spread over exceptionally long
	//  cables, the default value of MACLCON2 may
	//  need to be increased.

	// Ignoring for now ...

	// 9. Program the local MAC address into the
	//  MAADR1:MAADR6 registers.
	enc28j60_write_mac(config.mac);
}

/*********************************************
 * ENC28J60 Networking
 *********************************************/

u8 enc28j60_packet_count(void)
{
	enc28j60_bank_select(ENC28J60_BANK_1);
	return enc28j60_eth_rcr(ENC28J60_BK1_EPKTCNT);
}

void enc28j60_pkt_prepare(enc28j60_pkt_t *pkt, uint8_t *da, u16 type)
{
	memcpy(pkt->eth_pkt.hdr.da, da, 6);
	memcpy(pkt->eth_pkt.hdr.sa, config.mac, 6);

	pkt->eth_pkt.hdr.type = BSWAP16(type);
	pkt->cb = 0x00;
}

void enc28j60_write(const enc28j60_pkt_t *pkt, u16 len)
{
	register u16 reg16;

	enc28j60_bank_select(ENC28J60_BANK_0);

	// 1. Appropriately program the ETXST Pointer to
	//  point to an unused location in memory. It will
	//  point to the per packet control byte. In the
	//  example, it would be programmed to 0120h. It is
	//  recommended that an even address be used for
	//  ETXST.

	enc28j60_wcr(ENC28J60_BK0_ETXSTL, (u8) config.tx_buff_start);
	enc28j60_wcr(ENC28J60_BK0_ETXSTH, (u8) (config.tx_buff_start >> 8));

	// 2. Use the WBM SPI command to write the per
	//  packet control byte, the destination address, the
	//  source MAC address, the type/length and the
	//  data payload.

	// Enables auto increment of the write address, and sets
	//  the buffer start address
	enc28j60_bfs(ENC28J60_ECON2, ENC28J60_ECON2_AUTOINC);
	enc28j60_set_wbm_address(config.tx_buff_start);

	// Writes the packet
	enc28j60_wbm((u8 *) pkt, sizeof (enc28j60_pkt_t));
	enc28j60_wbm(pkt->eth_pkt.payload, len);

	// 3. Appropriately program the ETXND Pointer. It
	//  should point to the last byte in the data payload.
	//  In the example, it would be programmed to
	//  0156h.

	// Since the Write 'Buffer Memory Command', incremented the EWRPT register
	//  and by reading it's value, and removing 1 from it, we will get the address
	//  of the final byte in the packet
	reg16 = enc28j60_eth_rcr(ENC28J60_BK0_EWRPTH);
	reg16 <<= 8;
	reg16 |= enc28j60_eth_rcr(ENC28J60_BK0_EWRPTL);
	reg16 -= 1;

	// Stores the address inside the ETXND pointer
	enc28j60_wcr(ENC28J60_BK0_ETXNDL, (u8) reg16);
	enc28j60_wcr(ENC28J60_BK0_ETXNDH, (u8) (reg16 >> 8));

	// 4. Clear EIR.TXIF, set EIE.TXIE and set EIE.INTIE
	//  to enable an interrupt when done (if desired).

	enc28j60_bfc(ENC28J60_EIR, _BV(ENC28J60_EIR_TXIF));

	// 5. Start the transmission process by setting
	//  ECON1.TXRTS.

	enc28j60_bfs(ENC28J60_ECON1, _BV(ENC28J60_ECON1_TXRTS));

	// 6. Wait for the TXRTS bit to be cleared
	enc28j60_eth_wait_until_clear(ENC28J60_ECON1, ENC28J60_ECON1_TXRTS);
}

bool enc28j60_read(enc28j60_pkt_t *pkt)
{
	u16 read_pointer, next_packet_pointer;
	enc28j60_status_vector_t status_vector;

	// Checks if there are any packets available for reading
	u8 pkt_count = enc28j60_packet_count();
	if (pkt_count == 0) return false;

	// Selects bank 0
	enc28j60_bank_select(ENC28J60_BANK_0);

	// Gets the packed read position
	read_pointer = (u8) enc28j60_eth_rcr(ENC28J60_BK0_ERXRDPTH);
	read_pointer <<= 8;
	read_pointer |= (u8) enc28j60_eth_rcr(ENC28J60_BK0_ERXRDPTL);

	// Enables auto increment and sets the buffer read pointer
	enc28j60_bfs(ENC28J60_ECON2, ENC28J60_ECON2_AUTOINC);
	enc28j60_set_rbm_address(read_pointer);

	// Reads the next packet pointer, and the status vector and the packet
	enc28j60_rbm((u8 *) &next_packet_pointer, sizeof (u16));
	enc28j60_rbm((u8 *) &status_vector, sizeof (enc28j60_status_vector_t));
	enc28j60_rbm((u8 *) &pkt->eth_pkt, status_vector.rbc);

	// Increments the packet read position, which will make new space
	//  available for packets
	enc28j60_wcr(ENC28J60_BK0_ERXRDPTL, (u8) next_packet_pointer);
	enc28j60_wcr(ENC28J60_BK0_ERXRDPTH, (u8) (next_packet_pointer >> 8));

	// Decrements the packet counter
	enc28j60_bfs(ENC28J60_ECON2, (_BV(ENC28J60_ECON2_PKTDEC)));

	return true;
}


void enc28j60_poll(u8 *buffer)
{
	enc28j60_pkt_t *pkt = (enc28j60_pkt_t *) buffer;
	if (!enc28j60_read(pkt)) return;

	// Checks the packet type, so we can handle the packet
	//  accordingly
	switch (BSWAP16(pkt->eth_pkt.hdr.type))
	{
	case ETHERNET_PKT_TYPE_IPV4:
		enc28j60_handle_ipv4(pkt);
		break;
	case ETHERNET_PKT_TYPE_ARP:
		enc28j60_handle_arp(pkt);
		break;
	default:
		break;
	}
}

/*********************************************
 * ENC28J60 Networking ( ARP )
 *********************************************/

void enc28j60_handle_arp_reply(enc28j60_pkt_t *pkt)
{
	arp_pkt_t *arp_pkt = (arp_pkt_t *) pkt->eth_pkt.payload;

	// Checks if the hardware address length is correct
	if (arp_pkt->hdr.hln != 6) return;
}

void enc28j60_handle_arp_request(enc28j60_pkt_t *pkt)
{
	arp_pkt_t *arp_pkt = (arp_pkt_t *) pkt->eth_pkt.payload;

	// Checks if the hardware address length is correct
	if (arp_pkt->hdr.hln != 6) return;

	// Checks if the request is IPv4 or IPv6
	switch (arp_pkt->hdr.pln)
	{
	case 4:
	{
		arp_payload_ipv4_t *arp_ipv4_t = (arp_payload_ipv4_t *) arp_pkt->payload;

		// Checks if our hardware address, or our protocol address is
		//  present in the packet, and respond accordingly
		if (memcmp(arp_ipv4_t->tpa, config.ipv4_address, 4) == 0 || memcmp(arp_ipv4_t->tha, config.mac, 6) == 0)
		{
			// Prepares the ARP reply, by first copying the source addresses
			//  into the destination ones
			memcpy(arp_ipv4_t->tha, arp_ipv4_t->sha, 6);
			memcpy(arp_ipv4_t->tpa, arp_ipv4_t->spa, 4);

			// Modifies the opcode, and puts our info into the source addresses
			arp_pkt->hdr.op = BSWAP16(ARP_PKT_OP_REPLY);
			memcpy(arp_ipv4_t->sha, config.mac, 6);
			memcpy(arp_ipv4_t->spa, config.ipv4_address, 4);

			// Writes the response packet
			enc28j60_write(pkt, sizeof (arp_pkt_t) + sizeof (arp_payload_ipv4_t));
		}

		break;
	}
	case 6:
		break;
	default:
		break;
	}
}

void enc28j60_handle_arp(enc28j60_pkt_t *pkt)
{
	arp_pkt_t *arp_pkt = (arp_pkt_t *) pkt->eth_pkt.payload;

	switch (BSWAP16(arp_pkt->hdr.op))
	{
	case ARP_PKT_OP_REQUEST:
		enc28j60_handle_arp_request(pkt);
		break;
	case ARP_PKT_OP_REPLY:
		enc28j60_handle_arp_reply(pkt);
		break;
	default:
		break;
	}
}

/*********************************************
 * ENC28J60 Networking ( IP )
 *********************************************/

void enc28j60_ipv4_prepare(ip_pkt_t *ip_pkt, u8 *ipv4)
{
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;

	memcpy(ip_ipv4->da, ipv4, 4);
	memcpy(ip_ipv4->sa, config.ipv4_address, 4);

	ip_pkt->hdr.ihl = 5;
	ip_pkt->hdr.ver = 4;
	ip_pkt->hdr.ttl = 60;
	ip_pkt->hdr.tos = (IP_HDR_TOS_PRECEDENCE(IP_HDR_TOS_PRECEDENCE_ROUTINE));
}

void enc28j60_ipv4_finish(ip_pkt_t *ip_pkt)
{
	ip_pkt->hdr.cs = 0;
	ip_pkt->hdr.cs = checksum_oc16((u16 *) ip_pkt, (ip_pkt->hdr.ihl * 2));
}

void enc28j60_handle_ipv4(enc28j60_pkt_t *pkt)
{
	ethernet_pkt_t *eth_pkt = (ethernet_pkt_t *) &pkt->eth_pkt;
	ip_pkt_t *ip_pkt = (ip_pkt_t *) eth_pkt->payload;
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;

	// Checks if the checksum is valid, else discard
	//  the IP packet
	if (checksum_oc16((u16 *) ip_pkt, (ip_pkt->hdr.ihl * 2)) != 0xFFFF)
		return;

	// Checks if the packet matches our IPv4, or is an broadcast packet
	if (memcmp(ip_ipv4->da, config.ipv4_address, 4) != 0
			&& memcmp(ip_ipv4->da, config.ipv4_broadcast, 4) != 0) return;

	// Checks the protocol, and calls the required handler
	switch (ip_pkt->hdr.proto)
	{
	case IP_HDR_PROTO_ICMP:
		enc28j60_handle_icmp_ipv4(pkt);
		break;
	case IP_HDR_PROTO_UDP:
		enc28j60_handle_ipv4_udp(pkt);
		break;
	case IP_HDR_PROTO_TCP:
		break;
	}
}

/*********************************************
 * ENC28J60 Networking ( ICMP )
 *********************************************/

void enc28j60_handle_icmp_ipv4(enc28j60_pkt_t *pkt)
{
	ethernet_pkt_t *eth_pkt = (ethernet_pkt_t *) &pkt->eth_pkt;
	ip_pkt_t *ip_pkt = (ip_pkt_t *) eth_pkt->payload;
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;
	icmp_pkt_t *icmp_pkt = (icmp_pkt_t * ) ip_ipv4->payload;

	// Validates the checksum
	if (icmp_calc_cs(icmp_pkt, BSWAP16(ip_pkt->hdr.tl) - (ip_pkt->hdr.ihl * 4)) != 0xFFFF)
		return;

	// Checks the ICMP type, and how to handle it
	switch (icmp_pkt->hdr.type)
	{
	case ICMP_TYPE_ECHO:
	{
		enc28j60_pkt_prepare(pkt, eth_pkt->hdr.sa, ETHERNET_PKT_TYPE_IPV4);

		enc28j60_ipv4_prepare(ip_pkt, ip_ipv4->sa);
		ip_pkt->hdr.proto = IP_HDR_PROTO_ICMP;
		enc28j60_ipv4_finish(ip_pkt);

		icmp_pkt->hdr.type = ICMP_TYPE_ECHO_REPLY;
		icmp_pkt->hdr.code = ICMP_ERM_CODE;
		icmp_pkt->hdr.cs = 0;
		icmp_pkt->hdr.cs = icmp_calc_cs(icmp_pkt, BSWAP16(ip_pkt->hdr.tl) - (ip_pkt->hdr.ihl * 4));

		enc28j60_write(pkt, BSWAP16(ip_pkt->hdr.tl));
		break;
	}
	}
}

/*********************************************
 * ENC28J60 Networking ( UDP )
 *********************************************/

void enc28j60_ipv4_udp_prepare(ip_pkt_t *ip_pkt, udp_pkt_t *udp_pkt, u16 port)
{
	ip_ipv4_body_t *ip_ipv4 = (ip_ipv4_body_t *) ip_pkt->payload;
	ip_pkt->hdr.proto = IP_HDR_PROTO_UDP;
	ip_pkt->hdr.f = BSWAP16(0x00);
	ip_pkt->hdr.id = BSWAP16(0x00);
	ip_pkt->hdr.tl = BSWAP16((ip_pkt->hdr.ihl * 4) + sizeof (udp_pkt_t) + BSWAP16(udp_pkt->hdr.l));

	udp_pkt->hdr.sp = BSWAP16(0);
	udp_pkt->hdr.dp = BSWAP16(port);
	udp_pkt->hdr.cs = 0;
	udp_pkt->hdr.cs = udp_calc_cs(udp_pkt, ip_ipv4->da, ip_ipv4->sa, 4, 4, IP_HDR_PROTO_UDP);
}

void enc28j60_handle_ipv4_udp(enc28j60_pkt_t *pkt)
{
	udp_packet_callback(pkt);
}
