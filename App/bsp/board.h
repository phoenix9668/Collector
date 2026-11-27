/**
 * @file    board.h
 * @brief   Board-level GPIO: LEDs, CC1101 power and RF switch, 4G module
 *          control lines. Pin names come from Collector.ioc (main.h).
 */
#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#include "main.h"

static inline void board_write(GPIO_TypeDef *port, uint32_t pin, bool high)
{
    if (high)
    {
        LL_GPIO_SetOutputPin(port, pin);
    }
    else
    {
        LL_GPIO_ResetOutputPin(port, pin);
    }
}

/* STATUS_LED (PA8): the external status lamp on P4, owned by station_status.c. */
static inline void board_status_led(bool on) { board_write(STATUS_LED_GPIO_Port, STATUS_LED_Pin, on); }
/* LED2 (PA11): on while a downlink frame is handled. */
static inline void board_led2(bool on) { board_write(LED2_GPIO_Port, LED2_Pin, on); }
/* LED_GREEN (PA12): fault indicator before a fatal reset. */
static inline void board_led_fault(bool on) { board_write(LED_GREEN_GPIO_Port, LED_GREEN_Pin, on); }

/* CC1101 supply switch (PA15, high = on). */
static inline void board_rf_power(bool on) { board_write(PA15_GPIO_Port, PA15_Pin, on); }

/* RF front-end switch: TX_EN (PA1) and RX_EN (PB2). */
static inline void board_rf_path_rx(void)
{
    LL_GPIO_ResetOutputPin(TX_EN_GPIO_Port, TX_EN_Pin);
    LL_GPIO_SetOutputPin(RX_EN_GPIO_Port, RX_EN_Pin);
}
static inline void board_rf_path_off(void)
{
    LL_GPIO_ResetOutputPin(TX_EN_GPIO_Port, TX_EN_Pin);
    LL_GPIO_ResetOutputPin(RX_EN_GPIO_Port, RX_EN_Pin);
}

/* CC1101 SPI chip select (PA2, active low) and lines read as GPIO. */
static inline void board_rf_cs(bool active) { board_write(SPI1_CS_GPIO_Port, SPI1_CS_Pin, !active); }
static inline bool board_rf_miso_high(void) { return LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_6) != 0u; }
static inline bool board_rf_gdo0_high(void) { return LL_GPIO_IsInputPinSet(GDO0_GPIO_Port, GDO0_Pin) != 0u; }

/* 4G module PWRKEY (PB3) and RESET (PB8); asserted = driven low. */
static inline void board_modem_pwrkey(bool asserted)
{
    board_write(USR4G_POWER_KEY_GPIO_Port, USR4G_POWER_KEY_Pin, !asserted);
}
static inline void board_modem_reset(bool asserted)
{
    board_write(EC600X_RST_GPIO_Port, EC600X_RST_Pin, !asserted);
}

#endif /* BOARD_H */
