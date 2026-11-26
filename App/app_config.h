/**
 * @file    app_config.h
 * @brief   Every tunable of the Collector application in one place.
 *
 * Peripheral setup (pins, clocks, baud rates, RTOS objects) belongs to
 * Collector.ioc. This header only holds application behaviour.
 */
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#ifndef APP_FW_VERSION
#define APP_FW_VERSION "dev"
#endif

/* ---------------------------------------------------------------------------
 * Feature switches
 * ------------------------------------------------------------------------- */

/* Read the SGM58031 on the power management board and send B2 frames, as
 * the RS485 local-station setup needs (Collector-QuecPython, branch
 * feature/rs485-local: the ESP32 adapter has no I2C, so only this board can
 * report battery and supply voltages).
 * Safe without the I2C jumper on the power board: the board reports
 * "##SGM58031 not found##" once and keeps running. Before 2.0.0 enabling it
 * without the jumper made the board reset every 20 minutes. */
#ifndef APP_SGM58031_ENABLE
#define APP_SGM58031_ENABLE 1
#endif

/* Mirror every B0 frame on LPUART1 (PB10), as the firmware always did. */
#ifndef APP_B0_MIRROR_LPUART1
#define APP_B0_MIRROR_LPUART1 1
#endif

/* Verbose "[dbg]" lines on USART1. Set by the DebugLog CMake preset. */
#ifndef APP_DEBUG_LOG
#define APP_DEBUG_LOG 0
#endif

/* ---------------------------------------------------------------------------
 * CC1101 radio (must match the collars)
 * ------------------------------------------------------------------------- */
#define APP_RF_ADDRESS            0xEFu
#define APP_RF_SYNC_WORD          0x1234u
#define APP_RF_START_DELAY_MS     (20u * 1000u)          /* let the 4G module boot first */
#define APP_RF_REINIT_PERIOD_MS   (2u * 60u * 60u * 1000u) /* power-cycle the radio */
#define APP_RF_POWER_OFF_MS       100u
#define APP_RF_PACKET_TIMEOUT_MS  250u   /* 255 B at 4.8 kB/s is ~53 ms */
#define APP_RF_RETRY_MS           (10u * 1000u)          /* after a failed init */

/* ---------------------------------------------------------------------------
 * 4G module (EC600X) or ESP32 adapter on USART1
 * ------------------------------------------------------------------------- */
#define APP_MODEM_PWRKEY_PULSE_MS       2000u
#define APP_MODEM_PWRKEY_PERIOD_MS      (2u * 60u * 60u * 1000u)
#define APP_MODEM_HEARTBEAT_TIMEOUT_MS  (10u * 60u * 1000u)
#define APP_MODEM_RESET_PULSE_MS        500u

/* Longest downlink frame accepted (ASCII hex); longer ones are dropped. */
#define APP_LINK_FRAME_MAX              64u

/* ---------------------------------------------------------------------------
 * Supply monitoring (SGM58031, B2 frames)
 * ------------------------------------------------------------------------- */
#define APP_B2_FIRST_DELAY_MS   (60u * 1000u)
#define APP_B2_PERIOD_MS        (20u * 60u * 1000u)

/* ---------------------------------------------------------------------------
 * Housekeeping
 * ------------------------------------------------------------------------- */
#define APP_LED_BLINK_PERIOD_MS     4000u   /* LED1 toggles: firmware alive */
#define APP_WDG_CHECKIN_TIMEOUT_MS  10000u  /* a task silent this long stops IWDG refresh */

/* ---------------------------------------------------------------------------
 * Data EEPROM layout (offsets from 0x08080000)
 * ------------------------------------------------------------------------- */
#define APP_NVM_COLLECTOR_ID_OFFSET 0x0000u

#endif /* APP_CONFIG_H */
