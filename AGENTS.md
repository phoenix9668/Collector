# Project Development Guide

## Context

- Target: STM32L051C8, Cortex-M0+, 64 KB Flash and 8 KB RAM, 12 MHz HSE,
  32 MHz SYSCLK, LSE for the RTC.
- Firmware: C11, CMake/Ninja, arm-none-eabi GCC (STM32Cube for VS Code
  bundles), FreeRTOS 10.2 through CMSIS-RTOS2.
- HAL and LL are intentionally mixed (LL: GPIO, DMA, USART1, LPUART1, SPI1;
  HAL: RCC, IWDG, RTC, CRC, I2C1). Preserve the selected peripheral API.
- Use Chinese for project discussion unless the user requests another language.
- Follow the user's requested scope. A planning or review task does not
  authorize implementing every issue discovered during that task.
- The Keil MDK project was removed; its last state is the `mdk-final` tag.

## Ownership

- `Collector.ioc` owns pins, clocks, peripherals and the RTOS objects (the
  three tasks and `linkTxMutex`, all static, task code "As external"). Change
  them there and regenerate (STM32CubeMX 6.18+, headless works:
  `STM32CubeMX.exe -q <script>` with `config load`, `project generate`,
  `exit`). Headless generation does not recompute derived values: set the
  I2C `Timing` explicitly and check the generated code.
- `Core/`, `Drivers/`, `Middlewares/`, `cmake/` and the linker script are
  generated. In `Core/` only USER CODE sections may be edited, and they only
  call into `App/`:
  - `main.c`: `App_Init()` in USER CODE 2, `App_FatalError()` in
    `Error_Handler()`.
  - `stm32l0xx_it.c`: the EXTI3/4, DMA1_Channel2_3, USART1, LPUART1 and
    HardFault hooks declared in `App/app_isr.h`.
  - `rtc.c`: keep the calendar across resets (`Check_RTC_BKUP`, DR0 magic).
  - `FreeRTOSConfig.h` USER CODE Defines: shrinks `configTOTAL_HEAP_SIZE`
    (everything is static; CubeMX enforces a larger minimum).
- `App/` is owned code; `CMakeLists.txt` (top level) is user-owned and adds
  `App/`, the post-build hex/bin and `-Og` for Debug.

## RAM and stacks

- Release uses about 6.8 KB of 8 KB. `Tools/check_size.py` (also in CI)
  fails above 7368 B RAM or 48 KB flash.
- CMSIS-RTOS2 fixes `configMAX_PRIORITIES` at 56 (1.1 KB of ready lists);
  do not override it, the generated tasks use CMSIS priorities.
- Task stacks are 160 words each. The deepest paths are the log calls
  (`emit()` keeps a 96-byte line on the stack). Worst case measured with
  `-fcallgraph-info=su` is about 420 B plus context; keep that margin when
  adding work to a task.
- No `printf`/`malloc`: use `util/fmt.h` and `util/log.h`. Log functions
  block on the USART1 mutex: task context only, never in an ISR.

## Compatibility (do not change without the 4G / station side)

- B0 frames are byte-for-byte the old layout (206 B for a 191-byte collar
  payload, CRC-32 over bytes 0..L+8, `0D 0A` tail). The 4G firmware, the ESP32
  adapter and the PC station sync on `0x0B` + function byte and take fixed
  sizes (B0 206, B2 9).
- B2: battery = AIN2-AIN3 (x11), power = AIN0-AIN1 (x21), raw big-endian
  codes, negative codes clamped to 0.
- Downlink: ASCII hex frames delimited by line idle, the 9-byte `Heartbeat`,
  and the `##...##` reply wording.
- Radio: 433.0 MHz, sync 0x1234, address 0xEF, 24-byte preamble; must match
  the collars (`../RFID_CC1101_433MHz_V3`).
- SGM58031 behaviour follows the 4.1.6 reference driver in
  `../Collector-QuecPython` (branch `feature/rs485-local`,
  `src/collector_sensors.py`); I2C errors never reach `Error_Handler()`.

## Checks

- Firmware: `cmake --preset Release` / `Debug` / `DebugLog`, then
  `cmake --build --preset <name>`; no warnings in `App/`.
- Host tests: `python Tools/host_tests.py` (unit tests plus the frame
  cross-check against `../Collector-QuecPython` when it is checked out).
- Hardware behaviour (radio timing, heartbeat reset, B2 values) needs the
  bench steps in README.md; say so when a change was not tried on a board.
