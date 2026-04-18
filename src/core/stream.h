#ifndef STREAM_H
#define STREAM_H

#include "samplepack.h"
#include "streamchannel.h"

#include <QObject>
#include <QVector>

class Stream : public QObject
{
    Q_OBJECT

public:
    explicit Stream(QObject *parent = nullptr);
    ~Stream() override;

    void appendSamples(const SamplePack& pack); 
    void clear();

    void setSampleWindow(int samples);
    int sampleWindow() const;

    int channelCount() const;
    StreamChannel* channel(int index);
    const StreamChannel* channel(int index) const;

    void setPaused(bool paused);
    bool isPaused() const;

signals:

private:
    void ensureChannelCount(int count);

    QVector<StreamChannel*> m_channels; // 存储所有通道指针的向量
    int m_sampleWindow; // 采样窗口，控制数据保留量
    bool m_paused; //暂停标志

};

#endif // STREAM_H
