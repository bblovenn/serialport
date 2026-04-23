#ifndef STREAMCHANNEL_H
#define STREAMCHANNEL_H

#include "ringbuffer.h"

#include <QColor>
#include <QString>
#include <QVector>

// StreamChannel 表示单条波形通道，例如 CH1、CH2。
// 它把显示属性（名称、颜色、是否可见）和实际采样缓存放在一起，方便 PlotWidget 绘制。
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
    RingBuffer m_buffer; // 当前通道的采样数据。
    QString m_name;      // 例如 CH1、CH2。
    QColor m_color;      // 绘图时使用的颜色。
    bool m_visible;      // 是否参与显示。
};

#endif // STREAMCHANNEL_H
