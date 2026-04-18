#ifndef STREAMCHANNEL_H
#define STREAMCHANNEL_H

#include "ringbuffer.h"

#include <QColor>
#include <QVector>
#include <QString>

//StreamChannel 是数据流通道类，管理单条采样曲线的数据和显示属性：

//成员	用途
//m_buffer	存储历史采样值，自动覆盖旧数据
//m_name	通道名称（如"通道1"）
//m_color	曲线颜色，用于绘图区分
//m_visible	显示/隐藏控制

//多通道数据可视化（如示波器、心电监护）
//每个通道独立存储最近N个采样点
//支持动态显示/隐藏和颜色区分

class StreamChannel
{
public:
    explicit StreamChannel(int capacity = 1000);

    void append(double value); 
    void clear(); 

    void setCapacity(int capacity);
    int capacity() const;
    int size() const;

    QVector<double>values() const;

    QString name() const;
    void setName(const QString &name);

    QColor color() const;
    void setColor(const QColor &color);

    bool isVisible() const;
    void setVisible(bool visible);

private:
    RingBuffer m_buffer; 
    QString m_name;              
    QColor m_color;              
    bool m_visible;              

};

#endif // STREAMCHANNEL_H
