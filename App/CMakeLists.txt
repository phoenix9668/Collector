# Application layer. Everything under App/ is owned by this project; STM32CubeMX
# never writes here. Core/, Drivers/ and Middlewares/ are generated from
# Collector.ioc and only call into App/ from their USER CODE sections.

set(APP_FW_VERSION "2.0.0")

option(APP_DEBUG_LOG "Verbose [dbg] lines on USART1 (not for production)" OFF)

add_library(collector_app OBJECT
    app_main.c
    bsp/aux_uart.c
    bsp/link_uart.c
    bsp/nvm.c
    bsp/rtc_clock.c
    bsp/sensor_i2c.c
    drivers/cc1101.c
    drivers/sgm58031.c
    protocol/crc32_hw.c
    protocol/downlink.c
    protocol/uplink.c
    services/housekeeping.c
    services/link_service.c
    services/modem_supervisor.c
    services/power_monitor.c
    services/radio_service.c
    services/station_status.c
    services/status_logic.c
    util/fmt.c
    util/log.c
    util/watchdog.c
)

# PUBLIC: the generated Core sources include app_main.h, app_isr.h and rtc_clock.h.
target_include_directories(collector_app PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/services
    ${CMAKE_CURRENT_SOURCE_DIR}/util
)

target_compile_definitions(collector_app PUBLIC
    APP_FW_VERSION="${APP_FW_VERSION}"
    $<$<BOOL:${APP_DEBUG_LOG}>:APP_DEBUG_LOG=1>
)

target_link_libraries(collector_app PUBLIC stm32cubemx)

# Owned code builds clean with extra warnings; generated code keeps CubeMX's flags.
target_compile_options(collector_app PRIVATE
    -Wextra -Wshadow -Wundef -Wdouble-promotion -Wformat=2 -Wstrict-prototypes -Wmissing-prototypes
    -Wno-unused-parameter
)

if(APP_DEBUG_LOG)
    message(WARNING "APP_DEBUG_LOG is ON: this image prints [dbg] lines on USART1. Not for production.")
endif()
