# Qt 串口波形监视器

一个基于 Qt Widgets 和 Qt Serial Port 的串口波形显示工具。它可以打开本机串口，接收 ASCII 数值数据，并实时绘制单通道或多通道波形，同时提供基础的串口收发日志面板。

这个项目的定位不是单纯的串口助手，而是一个上位机实时数据采集原型。当前版本已经完成串口通信、ASCII 多通道解析、数据缓存、波形渲染、通信日志和基础测试，后续可以扩展为 `Qt + STM32` 上下位机实时数据采集系统。

## 项目亮点

- 基于 Qt Serial Port 封装串口扫描、打开、关闭、发送和异常处理。
- 基于 `QIODevice` 抽象读取数据源，串口读取和演示数据可以复用同一套解析链路。
- 支持 ASCII 单通道和多通道数据解析，例如 `0.5`、`0.1,0.2,0.3`。
- 使用环形缓冲区维护采样窗口，避免长时间运行时采样数据无限增长。
- 使用 `QPainter` 自绘网格、坐标轴和多通道波形，支持暂停、清空、采样窗口调整和 Y 轴自动缩放。
- 提供核心数据结构测试和 UI 布局测试，便于持续验证基础行为。

## 功能特性

- 串口列表刷新、打开、关闭和异常提示
- 支持常用波特率选择
- ASCII 数值数据解析并实时绘制波形
- 单通道格式：`0.5`
- 多通道格式：`0.1,0.2,0.3`
- 接收日志即时显示，不依赖换行符
- 支持发送文本，并可自动追加 `\r\n`
- 可暂停接收、清空波形、调整采样窗口
- 支持固定 Y 轴范围和 Y 轴自动缩放

## 面试可讲点

这个项目可以按“上位机数据采集链路”来介绍：

```text
串口设备/演示数据
  -> QIODevice 数据源
  -> AsciiReader 文本解析
  -> SamplePack 多通道采样包
  -> Stream / RingBuffer 数据缓存
  -> PlotWidget 实时波形渲染
  -> MainWindow 状态控制和通信日志
```

可以重点说明：

- `SerialController` 将底层串口配置、读写状态和错误处理从界面层拆出来，降低 UI 与硬件通信的耦合。
- `AsciiReader` 只依赖 `QIODevice`，因此既能读取 `QSerialPort`，也能接入测试或演示数据源。
- `Stream` 和 `RingBuffer` 负责多通道采样窗口管理，绘图控件不直接管理串口数据生命周期。
- `PlotWidget` 使用自绘方式实现波形显示，比直接堆控件更能体现 Qt 绘图和坐标映射能力。
- 后续可继续加入自定义 UART 协议、CRC 校验、丢包统计、CSV 数据保存和 STM32 联调内容，将项目升级为完整上下位机系统。

## 运行环境

- Qt 5.14.2 或兼容的 Qt 5 版本
- Qt 模块：`core` `gui` `widgets` `serialport`
- Windows + MinGW 64-bit 已验证

## 使用 Qt Creator 构建

1. 打开 Qt Creator。
2. 选择 `Open Project`，打开 `serialport.pro`。
3. 选择带有 Qt Serial Port 模块的 Desktop Kit，例如 `Desktop Qt 5.14.2 MinGW 64-bit`。
4. 点击构建并运行。

## 使用 qmake 命令行构建

```powershell
cd ...\serialport
$env:Path = "...\Qt5.14.2\Tools\mingw730_64\bin;D:\Qt\Qt5.14.2\5.14.2\mingw73_64\bin;" + $env:Path
...\Qt5.14.2\5.14.2\mingw73_64\bin\qmake.exe serialport.pro
mingw32-make
```

## 使用 CMake 构建

本项目保留 qmake 工程文件 `serialport.pro`，同时提供 CMake 构建入口，便于在现代 Qt/C++ 工程环境中使用。

```powershell
cd D:\QTProject\serialport\serialport\serialport
$env:Path = "D:\Qt\Qt5.14.2\Tools\mingw730_64\bin;D:\Qt\Qt5.14.2\5.14.2\mingw73_64\bin;" + $env:Path
$env:CMAKE_PREFIX_PATH = "D:\Qt\Qt5.14.2\5.14.2\mingw73_64"
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

## GitHub Actions 自动构建

仓库提供 `.github/workflows/build.yml`，用于在 Windows 环境安装 Qt 5 并通过 CMake 构建项目。推送到 `master`、`main` 或创建 Pull Request 时会自动触发构建。

Windows 发布包生成流程见 [docs/release.md](docs/release.md)。

## 串口数据格式

程序按行解析数值数据。建议发送端每一帧以换行结尾：

```text
0.0
0.2
0.5
-0.2
```

多通道数据使用英文逗号分隔：

```text
0.1,0.8
0.2,0.6
0.3,0.4
```

如果只是查看通信日志，接收文本不需要换行也会立即显示；但要输出波形，数值数据仍建议带 `\n` 或 `\r\n`。

## 后续升级方向

建议按下面顺序继续增强项目深度：

1. 增加自定义串口协议，例如 `$DATA,<seq>,<timestamp>,<ch1>,<ch2>,<ch3>*<crc>`。
2. 增加 CRC 校验、错误帧统计、丢包统计和解析失败日志。
3. 新增 CSV 数据导出，保存采样时间、通道值和原始帧。
4. 新增 STM32 UART 示例，周期性上传 ADC 或传感器数据，并支持 Qt 下发控制命令。
5. 补充 CMake、GitHub Actions 和 Windows Release 打包说明。

通信协议设计详见 [docs/protocol.md](docs/protocol.md)。
STM32 联调说明详见 [docs/hardware.md](docs/hardware.md)。
STM32 UART 示例见 [firmware/stm32_uart_demo](firmware/stm32_uart_demo)。

## 项目结构

```text
src/
  core/       数据容器、环形缓冲区和多通道数据流
  plot/       波形绘制控件
  readers/    串口 ASCII 读取和演示数据读取
  serial/     串口打开、关闭、发送和错误处理
docs/
  protocol.md 串口通信协议设计说明
  hardware.md STM32 联调说明
  release.md Windows 发布包说明
firmware/
  stm32_uart_demo/ STM32 UART 数据上传示例
.github/workflows/
  build.yml GitHub Actions 自动构建配置
tests/
  core/       核心数据结构测试
  ui/         主窗口布局测试
assets/       应用图标等资源
CMakeLists.txt CMake 构建入口
serialport.pro qmake 构建入口
```

## 开发说明

- 生成文件不要提交，例如 `debug/` `release/` `Makefile*` `ui_*.h` `qrc_*.cpp`。
- Qt Creator 生成的 `*.pro.user*` 和 `build-*` 目录不应提交。
- 当前应用图标来自 `assets/app_icon.png`，通过 `resources.qrc` 打包进程序。
- 如果需要展示运行截图，建议放到 `docs/images/`，不要提交本地构建目录或 IDE 生成文件。

## License

本项目使用 MIT License，详见 [LICENSE](LICENSE)。
