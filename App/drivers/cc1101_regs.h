/**
 * @file    cc1101_regs.h
 * @brief   CC1101 register map, command strobes and SPI header bits.
 */
#ifndef CC1101_REGS_H
#define CC1101_REGS_H

/* Configuration registers */
#define CC1101_IOCFG2      0x00u
#define CC1101_IOCFG1      0x01u
#define CC1101_IOCFG0      0x02u
#define CC1101_FIFOTHR     0x03u
#define CC1101_SYNC1       0x04u
#define CC1101_SYNC0       0x05u
#define CC1101_PKTLEN      0x06u
#define CC1101_PKTCTRL1    0x07u
#define CC1101_PKTCTRL0    0x08u
#define CC1101_ADDR        0x09u
#define CC1101_CHANNR      0x0Au
#define CC1101_FSCTRL1     0x0Bu
#define CC1101_FSCTRL0     0x0Cu
#define CC1101_FREQ2       0x0Du
#define CC1101_FREQ1       0x0Eu
#define CC1101_FREQ0       0x0Fu
#define CC1101_MDMCFG4     0x10u
#define CC1101_MDMCFG3     0x11u
#define CC1101_MDMCFG2     0x12u
#define CC1101_MDMCFG1     0x13u
#define CC1101_MDMCFG0     0x14u
#define CC1101_DEVIATN     0x15u
#define CC1101_MCSM2       0x16u
#define CC1101_MCSM1       0x17u
#define CC1101_MCSM0       0x18u
#define CC1101_FOCCFG      0x19u
#define CC1101_BSCFG       0x1Au
#define CC1101_AGCCTRL2    0x1Bu
#define CC1101_AGCCTRL1    0x1Cu
#define CC1101_AGCCTRL0    0x1Du
#define CC1101_WOREVT1     0x1Eu
#define CC1101_WOREVT0     0x1Fu
#define CC1101_WORCTRL     0x20u
#define CC1101_FREND1      0x21u
#define CC1101_FREND0      0x22u
#define CC1101_FSCAL3      0x23u
#define CC1101_FSCAL2      0x24u
#define CC1101_FSCAL1      0x25u
#define CC1101_FSCAL0      0x26u
#define CC1101_RCCTRL1     0x27u
#define CC1101_RCCTRL0     0x28u
#define CC1101_FSTEST      0x29u
#define CC1101_PTEST       0x2Au
#define CC1101_AGCTEST     0x2Bu
#define CC1101_TEST2       0x2Cu
#define CC1101_TEST1       0x2Du
#define CC1101_TEST0       0x2Eu

/* Command strobes */
#define CC1101_SRES        0x30u
#define CC1101_SFSTXON     0x31u
#define CC1101_SXOFF       0x32u
#define CC1101_SCAL        0x33u
#define CC1101_SRX         0x34u
#define CC1101_STX         0x35u
#define CC1101_SIDLE       0x36u
#define CC1101_SAFC        0x37u
#define CC1101_SWOR        0x38u
#define CC1101_SPWD        0x39u
#define CC1101_SFRX        0x3Au
#define CC1101_SFTX        0x3Bu
#define CC1101_SWORRST     0x3Cu
#define CC1101_SNOP        0x3Du

/* Status registers (read with the burst bit set) */
#define CC1101_PARTNUM     0x30u
#define CC1101_VERSION     0x31u
#define CC1101_FREQEST     0x32u
#define CC1101_LQI         0x33u
#define CC1101_RSSI        0x34u
#define CC1101_MARCSTATE   0x35u
#define CC1101_WORTIME1    0x36u
#define CC1101_WORTIME0    0x37u
#define CC1101_PKTSTATUS   0x38u
#define CC1101_VCO_VC_DAC  0x39u
#define CC1101_TXBYTES     0x3Au
#define CC1101_RXBYTES     0x3Bu

/* Multi-byte registers */
#define CC1101_PATABLE     0x3Eu
#define CC1101_TXFIFO      0x3Fu
#define CC1101_RXFIFO      0x3Fu

/* SPI header bits */
#define CC1101_WRITE_BURST 0x40u
#define CC1101_READ_SINGLE 0x80u
#define CC1101_READ_BURST  0xC0u

/* Field masks */
#define CC1101_RXBYTES_COUNT    0x7Fu
#define CC1101_RXBYTES_OVERFLOW 0x80u
#define CC1101_LQI_CRC_OK       0x80u
#define CC1101_MARCSTATE_MASK   0x1Fu

/* MARCSTATE values the radio service looks at */
#define CC1101_STATE_IDLE           0x01u
#define CC1101_STATE_RX             0x0Du
#define CC1101_STATE_RX_END         0x0Eu
#define CC1101_STATE_RX_RST         0x0Fu
#define CC1101_STATE_RXFIFO_OVERFLOW 0x11u

#endif /* CC1101_REGS_H */
