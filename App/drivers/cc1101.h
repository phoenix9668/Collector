/**
 * @file    cc1101.h
 * @brief   CC1101 433 MHz transceiver on SPI1 (receive only).
 *
 * Radio settings (must match the collars): 433.0 MHz, GFSK, 76.8 kBaud with
 * Manchester coding, 30/32 sync bits, 24-byte preamble, whitening, variable
 * length, hardware CRC16, status bytes appended, address check with 0x00 and
 * 0xFF as broadcast. GDO0 (inverted) is low while a packet is on air; GDO2
 * (inverted) falls when the RX FIFO holds CC1101_RX_FIFO_THRESHOLD bytes.
 *
 * The driver does no locking: only the radio task may call it.
 */
#ifndef CC1101_H
#define CC1101_H

#include <stdbool.h>
#include <stdint.h>

#include "cc1101_regs.h"

#define CC1101_RX_FIFO_THRESHOLD 60u

/* Resets and configures the chip, leaves it idle. False if it does not
 * answer on SPI. */
bool cc1101_init(uint8_t address, uint16_t sync_word);

/* RF switch to RX and SRX. */
void cc1101_rx_start(void);

/* SIDLE + SFRX, RF switch off. */
void cc1101_rx_flush(void);

/* RXBYTES (read until two reads agree, per the CC1101 errata). Bit 7 set
 * means the RX FIFO overflowed. */
uint8_t cc1101_rx_bytes(void);

void cc1101_read_fifo(uint8_t *buf, uint8_t len);
uint8_t cc1101_read_fifo_byte(void);

uint8_t cc1101_marcstate(void);

/* Raw RSSI register value to dBm (offset 74 dB for 433 MHz / 76.8 kBaud). */
int16_t cc1101_rssi_dbm(uint8_t raw);

#endif /* CC1101_H */
