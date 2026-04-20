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

    ensureChannelCount(pack.channelCount());

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
    const QVector<QColor> colors = {
        QColor(80, 220, 120),
        QColor(255, 214, 90),
        QColor(80, 170, 255),
        QColor(255, 120, 150)
    };

    while (m_channels.size() < count) {
        const int channelIndex = m_channels.size();

        StreamChannel* streamChannel = new StreamChannel(m_sampleWindow);
        streamChannel->setName(QString("CH%1").arg(channelIndex + 1));
        streamChannel->setColor(colors[channelIndex % colors.size()]);

        m_channels.append(streamChannel);
    }
}
