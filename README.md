# Collector

体征监测基站（RFID 子板）固件：STM32L051C8（Cortex-M0+，64 KB Flash / 8 KB RAM）+ FreeRTOS（CMSIS-RTOS2）。

- CC1101 在 433 MHz 接收项圈数据，校验后以 **B0 帧**经 USART1 上送 4G 模组（EC600X）或 ESP32 适配板，并在 LPUART1 镜像一份；
- 读取电源管理子板上的 **SGM58031**，以 **B2 帧**上送电池 / 电源电压；
- 接收 `Heartbeat` 与下行命令，10 分钟无心跳时复位 4G 模组。

工具链为 **CMake + GCC**，在 VS Code 中用 ST 官方 **STM32Cube for Visual Studio Code** 扩展开发。最后一个 Keil MDK 版本留档于标签 `mdk-final`。

## 开发环境

1. 安装 VS Code 与扩展 `stmicroelectronics.stm32-vscode-extension`（打开工程时会提示）。扩展会下载 GNU Tools for STM32、CMake、Ninja、ST-LINK GDB server 等，无需另装 STM32CubeCLT。
2. 用 VS Code 打开仓库根目录，按提示选择 CMake 配置预设：

| 预设 | 用途 |
|---|---|
| `Debug` | 调试（`-Og -g3`） |
| `Release` | 量产（`-Os`） |
| `DebugLog` | 调试，并在 USART1 输出 `[dbg]` 诊断行（不可用于量产） |

3. 编译：状态栏 *Build*，或命令行：

```powershell
cmake --preset Release
```

```powershell
cmake --build --preset Release
```

产物在 `build/<预设>/`：`Collector.elf`、`Collector.hex`、`Collector.bin`、`Collector.map`。

4. 调试 / 烧录：*运行和调试* 中选择 `Debug (ST-Link)`（下载并停在 `main`）或 `Flash and run (ST-Link)`。也可用 STM32CubeProgrammer 烧录 `Collector.hex`。

5. 外设配置改动一律在 `Collector.ioc`（STM32CubeMX 6.18 或更新）中完成，然后 *GENERATE CODE*。生成器只改 `Core/`、`Drivers/`、`Middlewares/`、`cmake/stm32cubemx/` 与链接脚本，`App/` 不受影响。

## 目录结构

```
Collector.ioc            CubeMX 工程：引脚、时钟、外设、FreeRTOS 任务与互斥量
Core/ Drivers/ Middlewares/   CubeMX 生成；Core/ 只在 USER CODE 段调用 App/
cmake/  CMakeLists.txt  CMakePresets.json  STM32L051xx_FLASH.ld  startup_stm32l051xx.s
App/                     本工程自有代码
  app_config.h           全部可调参数与功能开关
  app_main.c             App_Init()、致命错误处理、FreeRTOS 钩子
  bsp/                   板级：GPIO、USART1/LPUART1、数据 EEPROM、RTC、传感器 I2C
  drivers/               CC1101、SGM58031
  protocol/              B0/B2 构帧、下行解析、CRC-32（纯函数，可在 PC 上测试）
  services/              三个任务：radio / link / housekeeping，及 4G 模组看护、电压监测
  util/                  格式化、日志、看门狗
Tests/                   PC 上运行的单元测试
Tools/                   测试运行、帧交叉校验、内存预算检查脚本
Doc/                     硬件协议、器件资料、采购与生产文档
```

## 运行时结构

| 任务 | 优先级 | 栈 | 职责 |
|---|---|---|---|
| `Radio_Task` | High | 640 B | 独占 SPI1/CC1101；GDO 中断唤醒，按 RXBYTES 读 FIFO；CRC 校验后发 B0；每 2 h 断电重新初始化射频 |
| `Link_Task` | AboveNormal | 640 B | USART1 DMA 接收，按线路空闲分帧；处理 `Heartbeat` 与下行命令 |
| `Housekeeping_Task` | Normal | 640 B | 开机报告、4G 模组 PWRKEY / 心跳看护、SGM58031 与 B2、LED、看门狗 |

- USART1 发送用 DMA，由互斥量 `linkTxMutex` 串行化，B0 帧不会被日志插断。
- IWDG（约 28 s）只在三个任务都于 10 s 内签到时才喂，任一任务卡死都会复位。
- 空闲任务执行 `WFI`。所有 RTOS 对象静态分配，Release 版 RAM 约 6.8 KB / 8 KB。
- 致命错误（HAL 错误、HardFault、栈溢出）会把原因写入 RTC 备份寄存器后复位，下次开机在报告中给出。

## 串口协议（USART1，115200 8N1）

上行：

| 帧 | 格式 |
|---|---|
| B0 | `ID×4` `B0` 项圈数据（去掉末尾 CRC） `RSSI` `年 月 日 周` `时 分 秒` `CRC-32（大端，覆盖前面全部字节）` `0D 0A`；项圈数据 191 B 时整帧 206 B |
| B2 | `ID×4` `B2` `电池码 hi lo`（AIN2−AIN3） `电源码 hi lo`（AIN0−AIN1），共 9 B；电压 = 码 / 32768 × 4.096 × 11（电池）或 × 21（电源） |
| 状态 | `##…##\r\n` 文本，例如 `##Collector 2.0.0 ID=0B310001 reset=POR fault=0##` |

下行（ASCII 十六进制，以线路空闲分帧）：

| 命令 | 作用 / 回复 |
|---|---|
| `Heartbeat` | 喂 4G 模组看门狗（10 min 无心跳则 PB8 拉低 500 ms） |
| `E55E 01 aaaa dddddddd` | 写数据 EEPROM 偏移 `aaaa` 的 32 位字 → `##Write Memory Complete##` |
| `E55E 02 aaaa` | 读 → `dddddddd` 一行 + `##Read Memory Complete##` |
| `E55E 03 aaaa` | 写 0 → `##Erase Memory Complete##` |
| `<ID> A0` | 复位 → `##System Reset##` |
| `<ID> A8 YY MM DD WD hh mm ss` | 设置 RTC → `##Time Set Complete##` |
| `<ID> A9` | 读 RTC → `##RTC 2026-11-26 W4 02:30:59##` |

CollectorID 存在数据 EEPROM 偏移 `0x0000`，改写后复位生效。EEPROM 偏移须 4 字节对齐且小于 `0x0800`，否则回复 `##Memory Address Error##`。

## SGM58031 与 B2

电池、电源电压只能由本板读出（RS485 本地站方案中的 ESP32 适配板没有 I2C）。实现参考 Collector-QuecPython `feature/rs485-local` 分支的 4.1.6 驱动：每个通道一次单次转换（BUS_FLEX、轮询 OS、校验 MUX），负值钳为 0。

- 需在电源管理子板上插好跳线，把其 I2C 接到 RFID 子板。
- 开机 60 s 发第一帧 B2，之后每 20 min 一帧。
- 未插跳线时只输出一次 `##SGM58031 not found##`，不发 B2，板子照常运行；读数出错输出 `##SGM58031 read error N##`，本周期不发 B2。
- 关闭：`App/app_config.h` 中 `APP_SGM58031_ENABLE` 置 0。

## 测试

PC 上的单元测试覆盖构帧、下行解析、格式化、CRC 与 SGM58031 驱动（寄存器模型）。主机 C 编译器任选其一：gcc / clang，或 `python -m pip install ziglang`。

```powershell
python Tools/host_tests.py
```

若旁边有 `../Collector-QuecPython` 检出，脚本还会用 4G 固件自己的解析器 `src/collector_protocol.py` 逐字段解码固件生成的 B0/B2 帧（`Tools/crosscheck_frames.py`）。

GitHub Actions 对每次推送与 PR 编译三个预设、检查内存预算（`Tools/check_size.py`）、运行单元测试；在仓库 secret 中配置只读的 `QUECPYTHON_TOKEN` 后还会做交叉校验。

## 台架验收

USB-TTL 接 USART1（PA9 TX / PA10 RX，115200）：

1. 上电约 2 秒：`##LTE Module Power On Complete##`；约 20 秒：`##Collector 2.0.0 ID=... reset=POR fault=0##`（等 4G 模组 / ESP32 适配板就绪后才发）。
2. 插跳线：约 60 s 后出现 `0B …… B2 ……`，按上式换算与万用表一致；之后每 20 min 一帧。
3. 拔跳线：只出现 `##SGM58031 not found##`，20 min 后板子不复位。
4. 每 120 s 发 `Heartbeat`：PB8 无脉冲；停发 10 min：PB8 拉低 500 ms，并输出 `##LTE Module Reset (no heartbeat)##`。
5. 项圈发包：收到 206 B 的 B0，站点能解码。
6. 发 `<ID>A8…` 校时，再发 `<ID>A0` 复位，然后 `<ID>A9`：时间未丢。
