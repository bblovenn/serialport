#ifndef ABSTRACTREADER_H
#define ABSTRACTREADER_H

#include "core/samplepack.h"

#include <QObject>

// AbstractReader 是数据读取层的抽象基类。
// 真实串口、演示数据、后续可能的文件回放都可以继承它。
// 上层只监听 samplesReady，不需要知道数据来自哪里。
class AbstractReader : public QObject
{
    Q_OBJECT

public:
    explicit AbstractReader(QObject* parent = nullptr);
    ~AbstractReader() override;

public:
    // 启动/停止读取由子类实现，方便不同数据源使用不同机制。
    virtual void start() = 0;
    virtual void stop() = 0;

signals:
    // 读取器解析出一帧采样数据后，通过该信号交给 Stream。
    void samplesReady(const SamplePack& pack);
};

#endif // ABSTRACTREADER_H
