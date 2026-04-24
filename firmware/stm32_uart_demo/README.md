# STM32 UART Demo

这个目录提供一个轻量级 STM32 下位机示例，用于说明如何通过 UART 向 Qt 上位机周期性上传多通道采样数据，并接收上位机控制命令。

当前示例不是完整 CubeMX/Keil 工程，而是可迁移到 STM32 HAL 工程中的核心逻辑。实际移植时只需要把 `main.c` 中的硬件适配函数接到具体工程的 ADC、UART、GPIO 和定时器接口即可。

## 示例功能

- 每 20 ms 构造并发送一帧 `$DATA` 数据。
- 三个通道分别模拟 ADC 电压、温度和电位器百分比。
- 接收 `$CMD` 命令，支持 LED、PWM 和上传周期控制。
- 使用简单 XOR 校验生成帧尾校验值，便于上位机做错误帧过滤。

## 串口参数

| 参数   | 值     |
| ------ | ------ |
| 波特率 | 115200 |
| 数据位 | 8      |
| 校验位 | None   |
| 停止位 | 1      |
| 流控   | None   |

## 数据帧示例

```text
$DATA,15,123456,25.60,60.20,3.31*7A
```

字段说明见项目文档：[../../docs/protocol.md](../../docs/protocol.md)。

## 命令帧示例

```text
$CMD,LED,1*2B
$CMD,PWM,500*4F
$CMD,PERIOD,20*61
```

命令含义：

- `LED`：`1` 打开 LED，`0` 关闭 LED。
- `PWM`：设置 PWM 比较值或占空比参数。
- `PERIOD`：设置数据上传周期，单位为 ms。

## 移植步骤

1. 使用 CubeMX 创建 STM32 工程，启用 UART、ADC、GPIO 和可选 PWM。
2. 将 `main.c` 中的协议构帧、校验和命令处理逻辑复制到工程中。
3. 用 HAL 接口实现 `uart_send_text()`、`read_adc_voltage()`、`set_led()` 等硬件适配函数。
4. 保持 UART 参数与 Qt 上位机一致。
5. 连接 USB-TTL 后，在 Qt 上位机中打开串口并观察波形和日志。
