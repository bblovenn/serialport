#include "ringbuffer.h"

#include <QtGlobal>

RingBuffer::RingBuffer(int capacity)
    : m_capacity(qMax(1, capacity)), m_head(0), m_size(0), m_values(m_capacity)
{
}

void RingBuffer::setCapacity(int capacity)
{
    const int newCapacity = qMax(1, capacity);
    QVector<double> currentValues = values();

    // 缩小容量时保留最新的数据，丢弃最旧的数据。
    if (currentValues.size() > newCapacity) {
        currentValues = currentValues.mid(currentValues.size() - newCapacity);
    }

    // 重新分配固定长度数组，并从 0 开始顺序写回，简化后续索引计算。
    m_capacity = newCapacity;
    m_values = QVector<double>(m_capacity);
    m_head = 0;
    m_size = currentValues.size();

    for (int i = 0; i < m_size; ++i) {
        m_values[i] = currentValues[i];
    }
}

int RingBuffer::capacity() const
{
    return m_capacity;
}

int RingBuffer::size() const
{
    return m_size;
}

bool RingBuffer::isEmpty() const
{
    return m_size == 0;
}

void RingBuffer::append(double value)
{
    if (m_size < m_capacity) {
        // 未写满时，尾部位置由 head + size 计算得到。
        const int writeIndex = (m_head + m_size) % m_capacity;
        m_values[writeIndex] = value;
        ++m_size;
        return;
    }

    // 写满后覆盖最旧位置，并把 head 向前移动一格。
    m_values[m_head] = value;
    m_head = (m_head + 1) % m_capacity;
}

void RingBuffer::clear()
{
    m_size = 0;
    m_head = 0;
}

QVector<double> RingBuffer::values() const
{
    QVector<double> result;
    result.reserve(m_size);

    // 对外隐藏环形结构，始终按时间顺序导出数据。
    for (int i = 0; i < m_size; ++i) {
        const int index = (m_head + i) % m_capacity;
        result.append(m_values[index]);
    }

    return result;
}
