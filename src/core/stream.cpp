#include "stream.h"

#include <QColor>
#include <QtGlobal>

Stream::Stream(QObject* parent)
    : QObject(parent)
    , m_sampleWindow(1000)
    , m_paused(false)
{
}

Stream::~Stream()
{
    qDeleteAll(m_channels);
}

void Stream::appendSamples(const SamplePack& pack)
{
    if (m_paused || pack.isEmpty()) {
        return;
    }

    // 根据当前采样包的通道数自动补齐通道对象。
    ensureChannelCount(pack.channelCount());

    // SamplePack 是“同一时刻的多通道值”，这里拆成每个通道自己的历史序列。
    for (int i = 0; i < pack.channelCount(); ++i) {
        m_channels[i]->append(pack.value(i));
    }
}

void Stream::clear()
{
    for (StreamChannel* streamChannel : m_channels) {
        streamChannel->clear();
    }
}

void Stream::setSampleWindow(int samples)
{
    m_sampleWindow = qMax(1, samples);

    // 采样窗口变化时，同步调整所有已有通道的环形缓冲区容量。
    for (StreamChannel* streamChannel : m_channels) {
        streamChannel->setCapacity(m_sampleWindow);
    }
}

int Stream::sampleWindow() const
{
    return m_sampleWindow;
}

int Stream::channelCount() const
{
    return m_channels.size();
}

StreamChannel* Stream::channel(int index)
{
    return m_channels.value(index, nullptr);
}

const StreamChannel* Stream::channel(int index) const
{
    return m_channels.value(index, nullptr);
}

void Stream::setPaused(bool paused)
{
    m_paused = paused;
}

bool Stream::isPaused() const
{
    return m_paused;
}

void Stream::ensureChannelCount(int count)
{
    // 预置几种常用颜色，超过后循环使用。
    const QVector<QColor> colors = {
        QColor(80, 220, 120),
        QColor(255, 214, 90),
        QColor(80, 170, 255),
        QColor(255, 120, 150)
    };

    while (m_channels.size() < count) {
        const int channelIndex = m_channels.size();
        StreamChannel* streamChannel = new StreamChannel(m_sampleWindow);

        // 新通道使用 CH1、CH2... 的默认命名，后续可扩展为用户自定义。
        streamChannel->setName(QString("CH%1").arg(channelIndex + 1));
        streamChannel->setColor(colors[channelIndex % colors.size()]);

        m_channels.append(streamChannel);
    }
}
