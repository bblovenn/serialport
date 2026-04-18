#include "ringbuffer.h"

//环形缓冲区是一种固定容量的队列，新元素覆盖最旧元素
//数据流采样（如串口接收数据）
//与 samplepack 配合：两者都是数据容器，RingBuffer 侧重流式存储，samplepack 侧重采样封装


//确保对象创建时处于有效状态，避免后续操作访问非法内存
RingBuffer::RingBuffer(int capacity): // 构造函数声明，参数为 capacity
    m_capacity(qMax(1, capacity))  // 初始化成员变量 m_capacity，使用 qMax 确保容量至少为 1
{

}

void RingBuffer::setCapacity(int capacity)
{
    m_capacity = qMax(1, capacity);
    //如果 m_values 中已有元素超过新容量，移除多余的旧元素
    while (m_values.size() > m_capacity) {
        m_values.removeFirst(); // 移除最旧的元素，保持容量限制
    }
}

int RingBuffer::capacity() const
{
    return m_capacity;
}

int RingBuffer::size() const
{
    return m_values.size(); // 返回当前存储的元素数量，使用 QVector 的 size() 方法获取
}

bool RingBuffer::isEmpty() const
{
    return m_values.isEmpty();// 使用 QVector 的 isEmpty() 方法检查是否没有元素
}

void RingBuffer::append(double value)
{
    m_values.append(value); // 将新值添加到 QVector 的末尾
    //如果添加后元素数量超过容量，移除最旧的元素
    if (m_values.size() > m_capacity) {
        m_values.removeFirst(); // 移除最旧的元素，保持容量限制
    }
}

void RingBuffer::clear()
{
    m_values.clear(); // 清空 QVector，移除所有元素
}

QVector<double> RingBuffer::values() const
{
    return m_values; // 返回当前存储的所有元素，直接返回 QVector 对象
}
