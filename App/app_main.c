/**
 * @file    app_main.c
 * @brief   Start-up, fatal errors and the FreeRTOS hooks.
 *
 * Boot order (Core/Src/main.c):
 *   HAL_Init, SystemClock_Config, MX_xxx_Init   (CubeMX, Collector.ioc)
 *   App_Init()                                   (USER CODE 2, below)
 *   osKernelInitialize, MX_FREERTOS_Init         (tasks + mutex from the .ioc)
 *   osKernelStart -> Radio_Task, Link_Task, Housekeeping_Task
 */
#include "app_main.h"

#include "app_config.h"
#include "aux_uart.h"
#include "board.h"
#include "fmt.h"
#include "FreeRTOS.h"
#include "link_uart.h"
#include "log.h"
#include "nvm.h"
#include "rtc.h"
#include "rtc_clock.h"
#include "task.h"

/* CubeMX creates linkTxMutex with osMutexNew() and no control block, so it
 * comes from the FreeRTOS heap (configTOTAL_HEAP_SIZE is set in the USER CODE
 * Defines of FreeRTOSConfig.h). Too small a heap only shows on the board, as a
 * reset loop with fault code 4: catch it at build time instead. heap_4 adds an
 * 8-byte block header and may lose up to 7 bytes to alignment. */
_Static_assert(configTOTAL_HEAP_SIZE >= sizeof(StaticQueue_t) + 16u,
               "FreeRTOS heap too small for linkTxMutex (see FreeRTOSConfig.h USER CODE Defines)");

static uint8_t s_collector_id[4];
static uint32_t s_reset_flags;
static uint32_t s_last_fault;

void App_Init(void)
{
    /* Why did we reset? Read before anything can add a flag. */
    s_reset_flags = RCC->CSR;
    __HAL_RCC_CLEAR_RESET_FLAGS();
    s_last_fault = HAL_RTCEx_BKUPRead(&hrtc, RTC_CLOCK_BKP_FAULT);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_CLOCK_BKP_FAULT, APP_FAULT_NONE);

    uint32_t id = 0;
    (void)nvm_read_word(APP_NVM_COLLECTOR_ID_OFFSET, &id);
    s_collector_id[0] = (uint8_t)(id >> 24);
    s_collector_id[1] = (uint8_t)(id >> 16);
    s_collector_id[2] = (uint8_t)(id >> 8);
    s_collector_id[3] = (uint8_t)id;

    board_led1(false);
    board_led2(false);
    board_led_fault(false);
    board_modem_pwrkey(false);
    board_modem_reset(false);
    /* The radio task powers and configures the CC1101; until then its GDO
     * lines float, so keep their interrupts off. */
    NVIC_DisableIRQ(GDO0_EXTI_IRQn);
    NVIC_DisableIRQ(GDO2_EXTI_IRQn);
    board_rf_path_off();
    board_rf_power(false);

    link_uart_init();
    aux_uart_init();
}

const uint8_t *app_collector_id(void)
{
    return s_collector_id;
}

static const char *reset_cause(uint32_t csr)
{
    if (csr & RCC_CSR_LPWRRSTF)
    {
        return "LPWR";
    }
    if (csr & RCC_CSR_WWDGRSTF)
    {
        return "WWDG";
    }
    if (csr & RCC_CSR_IWDGRSTF)
    {
        return "IWDG";
    }
    if (csr & RCC_CSR_SFTRSTF)
    {
        return "SW";
    }
    if (csr & RCC_CSR_PORRSTF)
    {
        return "POR";
    }
    if (csr & RCC_CSR_OBLRSTF)
    {
        return "OBL";
    }
    if (csr & RCC_CSR_PINRSTF)
    {
        return "PIN";
    }
    return "?";
}

void app_report_boot(void)
{
    const uint8_t *id = s_collector_id;
    log_status("Collector %s ID=%02X%02X%02X%02X reset=%s fault=%lu", APP_FW_VERSION, id[0], id[1], id[2], id[3],
               reset_cause(s_reset_flags), (unsigned long)s_last_fault);
}

void App_FatalError(app_fault_t fault)
{
    __disable_irq();
    if (hrtc.Instance == RTC)
    {
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_CLOCK_BKP_FAULT, (uint32_t)fault);
    }
    board_led_fault(true);
    if (LL_USART_IsEnabled(USART1))
    {
        char line[32];
        (void)fmt_snprintf(line, sizeof(line), "##Fatal error %u##\r\n", (unsigned)fault);
        link_uart_write_polled(line);
    }
    /* Let the LED and the message be seen, then start over. */
    for (volatile uint32_t i = 0; i < 400000u; i++)
    {
    }
    NVIC_SystemReset();
    for (;;)
    {
    }
}

/* ------------------------------------------------------- FreeRTOS hooks */

/* Same declarations as the "Hook prototypes" in Core/Src/freertos.c. */
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

void vApplicationIdleHook(void)
{
    /* Sleep until the next interrupt (SysTick at the latest). */
    __WFI();
}

void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    App_FatalError(APP_FAULT_STACK_OVERFLOW);
}

void vApplicationMallocFailedHook(void)
{
    App_FatalError(APP_FAULT_MALLOC);
}
