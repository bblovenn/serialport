# Contributing

欢迎提交 issue、功能建议和 pull request。

这个项目的目标不是做一个“代码堆起来能跑”的示例，而是尽量保持它对学习者和后来维护者都友好，所以非常欢迎围绕可读性、稳定性和可复用性提出改进。

## 开发流程

1. Fork 仓库并创建独立分支
2. 使用 Qt Creator、qmake 或 CMake 确认项目可以构建
3. 修改代码时尽量保持模块边界清晰
4. 提交前运行相关测试或至少完成一次本地构建验证
5. 发起 pull request，并在描述里说明改动目的、影响范围和验证方式

## 模块边界建议

- `core/`：只处理数据结构和数据流
- `readers/`：只处理串口文本读取和解析接入
- `protocol/`：只处理协议字段和校验逻辑
- `serial/`：只处理串口控制
- `plot/`：只处理绘图
- `storage/`：只处理记录和导出
- `MainWindow`：只负责界面协调，不要把底层逻辑都堆进去

## 代码风格

- C++ 代码遵循 Qt 常见风格
- 类型名使用 `PascalCase`
- 成员变量使用 `m_` 前缀
- 注释优先解释“为什么这样做”
- 不要为了“聪明”牺牲可读性

## 提交建议

提交信息建议简洁说明改动目的，例如：

```text
Improve plot widget auto scale
Fix sequence wrap handling
Rewrite README for open-source release
```

## 不要提交这些内容

- 构建产物，例如 `debug/`、`release/`、`build/`
- IDE 用户配置，例如 `*.pro.user*`
- 本机路径、临时文件和测试输出

如果你不确定某项修改是否适合合入，先提 issue 讨论也完全没问题。
