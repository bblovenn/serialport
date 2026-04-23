# 串口波形监视器

一个基于 Qt Widgets 和 Qt Serial Port 的串口波形显示工具。它可以打开本机串口，接收 ASCII 数值数据，并实时绘制单通道或多通道波形，同时提供基础的串口收发日志面板。

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

## 运行环境

- Qt 5.14.2 或兼容的 Qt 5 版本
- Qt 模块：`core` `gui` `widgets` `serialport`
- Windows + MinGW 64-bit 已验证

## 使用 Qt Creator 构建

1. 打开 Qt Creator。
2. 选择 `Open Project`，打开 `serialport.pro`。
3. 选择带有 Qt Serial Port 模块的 Desktop Kit，例如 `Desktop Qt 5.14.2 MinGW 64-bit`。
4. 点击构建并运行。

## 使用命令行构建

```powershell
cd ...\serialport
$env:Path = "...\Qt5.14.2\Tools\mingw730_64\bin;D:\Qt\Qt5.14.2\5.14.2\mingw73_64\bin;" + $env:Path
...\Qt5.14.2\5.14.2\mingw73_64\bin\qmake.exe serialport.pro
mingw32-make
```

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

## 项目结构

```text
src/
  core/       数据容器、环形缓冲区和多通道数据流
  plot/       波形绘制控件
  readers/    串口 ASCII 读取和演示数据读取
  serial/     串口打开、关闭、发送和错误处理
tests/
  core/       核心数据结构测试
  ui/         主窗口布局测试
assets/       应用图标等资源
```

## 开发说明

- 生成文件不要提交，例如 `debug/` `release/` `Makefile*` `ui_*.h` `qrc_*.cpp`。
- Qt Creator 生成的 `*.pro.user*` 和 `build-*` 目录不应提交。
- 当前应用图标来自 `assets/app_icon.png`，通过 `resources.qrc` 打包进程序。

## License

本项目使用 MIT License，详见 [LICENSE](LICENSE)。
