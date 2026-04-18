#ifndef ABSTRACTREADER_H
#define ABSTRACTREADER_H

#include "core/samplepack.h"

#include <QObject>

//数据读取器的抽象基类，定义串口/数据源接口。

/*设计模式
模板方法模式 + 信号槽机制：

元素	说明
start() / stop()	纯虚函数，子类必须实现具体逻辑
samplesReady()	信号，读取到数据时发出，供上层处理
Q_OBJECT	启用 Qt 元对象系统，支持信号槽
数据流

数据源 (串口/传感器)
       ↓
AbstractReader 子类实现
       ↓
emit samplesReady(SamplePack)  // 发出信号
       ↓
上层 UI / Stream 处理数据*/

//AbstractReader 是数据采集层的抽象接口，定义统一的启动/停止操作，数据通过信号传递。

class AbstractReader : public QObject
{
    Q_OBJECT
public:
    explicit AbstractReader(QObject *parent = nullptr);
    ~AbstractReader() override;

public:
    virtual void start() = 0;
    virtual void stop() = 0;

signals:
    void samplesReady(const SamplePack& pack);  // 信号：有新数据
};

#endif // ABSTRACTREADER_H
