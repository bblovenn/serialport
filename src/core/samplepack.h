#ifndef SAMPLEPACK_H
#define SAMPLEPACK_H

#include <QVector>

// SamplePack 表示同一时刻的一组多通道采样值。
// 例如串口收到一行 "0.1,0.2,0.3"，会被解析成一个包含 3 个通道值的 SamplePack。
// 它只负责保存一次采样，不负责缓存历史数据，历史数据由 Stream/StreamChannel 管理。
class SamplePack
{
public:
    SamplePack();
    explicit SamplePack(const QVector<double>& values);

    // 返回当前采样包包含的通道数量。
    int channelCount() const;

    // 读取指定通道的采样值；越界时返回 0.0，避免调用方崩溃。
    double value(int channelIndex) const;

    // 返回一份完整数值拷贝，适合测试或一次性遍历。
    QVector<double> values() const;

    bool isEmpty() const;

private:
    QVector<double> m_values;
};

#endif // SAMPLEPACK_H
