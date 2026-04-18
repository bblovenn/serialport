#include "streamchannel.h"

/*代理模式 (Delegate)
代码大量使用委托模式，将核心操作委托给 m_buffer：


void StreamChannel::append(double value)
{
    m_buffer.append(value);  // 实际存储由 RingBuffer 处理
}

int StreamChannel::size() const
{
    return m_buffer.size();  // 查询也委托给 RingBuffer
}*/

/*与 RingBuffer 的关系
StreamChannel	底层调用
append()	m_buffer.append()
clear()	m_buffer.clear()
setCapacity()	m_buffer.setCapacity()
capacity()	m_buffer.capacity()
size()	m_buffer.size()
values()	m_buffer.values()
数据流示意

用户调用 append(1.5)
        ↓
StreamChannel::append(1.5)
        ↓
m_buffer.append(1.5)
        ↓
RingBuffer 存储，如果超容自动丢弃旧数据*/

/*StreamChannel 是高层封装，为数据可视化层提供：

统一的采样数据接口
显示属性（名称、颜色、可见性）
与绘图系统解耦，数据存储细节对上层透明*/


StreamChannel::StreamChannel(int capacity) // 构造函数，默认容量1000
        : m_buffer(capacity), m_color(Qt::black), m_visible(true)
{
}

void StreamChannel::append(double value) // 添加采样值
{
    m_buffer.append(value);
}

void StreamChannel::clear() // 清空数据
{
    m_buffer.clear();
}

void StreamChannel::setCapacity(int capacity) // 设置容量
{
    m_buffer.setCapacity(capacity);
}

int StreamChannel::capacity() const   // 获取容量
{
    return m_buffer.capacity();
}

int StreamChannel::size() const // 获取当前数据量
{
    return m_buffer.size();
}

QVector<double> StreamChannel::values() const  // 获取所有值
{
    return m_buffer.values();
}

QString StreamChannel::name() const  // 获取名称
{
    return m_name;
}

void StreamChannel::setName(const QString &name) // 设置名称
{
    m_name = name;
}

QColor StreamChannel::color() const // 获取颜色
{
    return m_color;
}

void StreamChannel::setColor(const QColor &color) // 设置颜色
{
    m_color = color;
}

bool StreamChannel::isVisible() const // 获取可见性
{
    return m_visible;
}

void StreamChannel::setVisible(bool visible) // 设置可见性
{
    m_visible = visible;
}
