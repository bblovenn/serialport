#ifndef STREAM_H
#define STREAM_H

#include "samplepack.h"
#include "streamchannel.h"

#include <QObject>
#include <QVector>

// Stream 负责管理整条波形数据流。
// 它接收 AsciiReader 解析出的 SamplePack，并按通道拆分到多个 StreamChannel 中。
// MainWindow 和 PlotWidget 通过 Stream 访问数据，不直接关心底层缓冲区实现。
class Stream : public QObject
{
    Q_OBJECT

public:
    explicit Stream(QObject* parent = nullptr);
    ~Stream() override;

    // 追加一次多通道采样；暂停时直接忽略输入。
    void appendSamples(const SamplePack& pack);
    void clear();

    // 控制每个通道最多保留多少个采样点。
    void setSampleWindow(int samples);
    int sampleWindow() const;

    int channelCount() const;
    StreamChannel* channel(int index);
    const StreamChannel* channel(int index) const;

    void setPaused(bool paused);
    bool isPaused() const;

private:
    // 根据输入数据通道数量自动创建缺失的通道。
    void ensureChannelCount(int count);

    QVector<StreamChannel*> m_channels; // 所有通道对象，Stream 负责释放。
    int m_sampleWindow;                 // 每个通道最多保留的采样点数。
    bool m_paused;                      // 暂停时忽略新输入。
};

#endif // STREAM_H
