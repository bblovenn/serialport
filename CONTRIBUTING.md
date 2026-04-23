# 贡献指南

欢迎提交 issue、功能建议和 pull request。

## 开发流程

1. Fork 仓库并创建独立分支。
2. 使用 Qt Creator 或命令行确认项目可以构建。
3. 修改代码时尽量保持模块边界清晰：
   - `core/` 只处理数据结构和数据流。
   - `readers/` 只处理数据读取和解析。
   - `serial/` 只处理串口控制。
   - `plot/` 只处理绘图。
4. 提交 PR 前运行构建，能跑测试时也请运行相关测试。

## 代码风格

- C++ 代码使用 Qt 风格，类名 PascalCase，成员变量使用 `m_` 前缀。
- 注释优先解释“为什么这样做”，避免重复描述代码本身。
- 不提交构建产物、IDE 用户配置和本机路径相关文件。

## 提交建议

提交信息建议简洁说明变更目的，例如：

```text
Add serial send panel
Fix partial serial writes
Improve plot widget scaling
```
