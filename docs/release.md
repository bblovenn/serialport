# Windows 发布包说明

本文档说明如何从源码生成 Windows 可执行发布包。发布包用于演示和交付，不应直接提交到 Git 仓库。

## 发布目标

生成一个可以在未安装 Qt Creator 的 Windows 电脑上运行的目录包：

```text
serialport-windows/
  serialport.exe
  Qt5Core.dll
  Qt5Gui.dll
  Qt5Widgets.dll
  Qt5SerialPort.dll
  platforms/
  styles/
  imageformats/
```

实际 DLL 数量以 `windeployqt` 输出为准。

## qmake Release 构建

```powershell
cd D:\QTProject\serialport\serialport\serialport
$env:Path = "D:\Qt\Qt5.14.2\Tools\mingw730_64\bin;D:\Qt\Qt5.14.2\5.14.2\mingw73_64\bin;" + $env:Path
D:\Qt\Qt5.14.2\5.14.2\mingw73_64\bin\qmake.exe serialport.pro CONFIG+=release
mingw32-make release
```

构建完成后，可执行文件通常位于：

```text
release/serialport.exe
```

## CMake Release 构建

```powershell
cd D:\QTProject\serialport\serialport\serialport
$env:Path = "D:\Qt\Qt5.14.2\Tools\mingw730_64\bin;D:\Qt\Qt5.14.2\5.14.2\mingw73_64\bin;" + $env:Path
$env:CMAKE_PREFIX_PATH = "D:\Qt\Qt5.14.2\5.14.2\mingw73_64"
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

构建完成后，可执行文件通常位于：

```text
build/serialport.exe
```

## 收集 Qt 运行库

使用 Qt 自带的 `windeployqt` 收集运行依赖：

```powershell
mkdir dist\serialport-windows
copy release\serialport.exe dist\serialport-windows\
windeployqt dist\serialport-windows\serialport.exe
```

如果使用 CMake 构建，则把 `copy` 命令中的 exe 路径替换为：

```powershell
copy build\serialport.exe dist\serialport-windows\
```

## 打包压缩文件

```powershell
Compress-Archive -Path dist\serialport-windows\* -DestinationPath dist\serialport-windows.zip -Force
```

生成的 `serialport-windows.zip` 可以上传到 GitHub Releases。

## 发布前检查

- 在一台没有 Qt Creator 的 Windows 电脑上解压运行。
- 打开程序后确认主窗口、图标、串口列表和通信日志正常显示。
- 使用串口助手或虚拟串口发送 `0.1,0.2,0.3`，确认波形正常绘制。
- 不要把 `dist/`、`release/`、`debug/`、`build/` 或 DLL 文件提交到 Git。

## GitHub Release 建议

Release 标题可以使用：

```text
Serialport Waveform Monitor v1.0.0
```

Release 说明建议包含：

- 支持串口打开、关闭、文本收发和异常提示。
- 支持 ASCII 单通道和多通道数据实时绘图。
- 支持采样窗口调整、暂停、清空和 Y 轴自动缩放。
- 附带通信协议、STM32 联调说明和下位机示例。
