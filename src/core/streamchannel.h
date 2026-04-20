#ifndef STREAMCHANNEL_H
#define STREAMCHANNEL_H

#include "ringbuffer.h"

#include <QColor>
#include <QString>
#include <QVector>

class StreamChannel
{
public:
    explicit StreamChannel(int capacity = 1000);

    void append(double value);
    void clear();

    void setCapacity(int capacity);
    int capacity() const;
    int size() const;

    QVector<double> values() const;

    QString name() const;
    void setName(const QString& name);

    QColor color() const;
    void setColor(const QColor& color);

    bool isVisible() const;
    void setVisible(bool visible);

private:
    RingBuffer m_buffer;
    QString m_name;
    QColor m_color;
    bool m_visible;
};

#endif // STREAMCHANNEL_H
