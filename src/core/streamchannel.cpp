#include "streamchannel.h"

StreamChannel::StreamChannel(int capacity)
    : m_buffer(capacity)
    , m_color(QColor(80, 220, 120))
    , m_visible(true)
{
}

void StreamChannel::append(double value)
{
    m_buffer.append(value);
}

void StreamChannel::clear()
{
    m_buffer.clear();
}

void StreamChannel::setCapacity(int capacity)
{
    m_buffer.setCapacity(capacity);
}

int StreamChannel::capacity() const
{
    return m_buffer.capacity();
}

int StreamChannel::size() const
{
    return m_buffer.size();
}

QVector<double> StreamChannel::values() const
{
    return m_buffer.values();
}

QString StreamChannel::name() const
{
    return m_name;
}

void StreamChannel::setName(const QString& name)
{
    m_name = name;
}

QColor StreamChannel::color() const
{
    return m_color;
}

void StreamChannel::setColor(const QColor& color)
{
    m_color = color;
}

bool StreamChannel::isVisible() const
{
    return m_visible;
}

void StreamChannel::setVisible(bool visible)
{
    m_visible = visible;
}
