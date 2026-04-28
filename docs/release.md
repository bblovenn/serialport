# Windows 发布说明

本文档说明如何从源码生成 Windows 可执行发布包。发布包用于演示和分发，不应直接提交到 Git 仓库。

## 发布目标

最终目录大致如下：

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

## 使用 qmake 生成 Release

```powershell
cd D:\QTProject\serialport\serialport\serialport
$env:Path = "D:\Qt\Qt5.14.2\Tools\mingw730_64\bin;D:\Qt\Qt5.14.2\5.14.2\mingw73_64\bin;" + $env:Path
D:\Qt\Qt5.14.2\5.14.2\mingw73_64\bin\qmake.exe serialport.pro CONFIG+=release
mingw32-make release
```

生成结果通常位于：

```text
release/serialport.exe
```

## 使用 CMake 生成 Release

```powershell
cd D:\QTProject\serialport\serialport\serialport
$env:Path = "D:\Qt\Qt5.14.2\Tools\mingw730_64\bin;D:\Qt\Qt5.14.2\5.14.2\mingw73_64\bin;" + $env:Path
$env:CMAKE_PREFIX_PATH = "D:\Qt\Qt5.14.2\5.14.2\mingw73_64"
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

生成结果通常位于：

```text
build/serialport.exe
```

## 收集 Qt 运行库

使用 `windeployqt` 收集运行依赖：

```powershell
mkdir dist\serialport-windows
copy release\serialport.exe dist\serialport-windows\
windeployqt dist\serialport-windows\serialport.exe
```

如果使用 CMake 构建，把 `copy` 的来源路径改成：

```powershell
copy build\serialport.exe dist\serialport-windows\
```

## 压缩发布包

```powershell
Compress-Archive -Path dist\serialport-windows\* -DestinationPath dist\serialport-windows.zip -Force
```

## 发布前检查

- 在未安装 Qt Creator 的 Windows 电脑上试运行
- 确认主窗口、图标、串口列表和通信日志显示正常
- 使用串口助手发送 ASCII 数据和 `$DATA` 协议帧，确认波形显示正常
- 验证“开始记录 / 停止记录”与 CSV 保存功能
- 不要把 `dist/`、`release/`、`debug/`、`build/` 或 DLL 文件提交到 Git
