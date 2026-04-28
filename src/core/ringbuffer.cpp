#include "ringbuffer.h"

#include <QtGlobal>

RingBuffer::RingBuffer(int capacity)
    : m_capacity(qMax(1, capacity))
    , m_head(0)
    , m_size(0)
    , m_values(m_capacity)
{
}

void RingBuffer::setCapacity(int capacity)
{
    const int newCapacity = qMax(1, capacity);
    QVector<double> currentValues = values(newCapacity);

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
        const int writeIndex = (m_head + m_size) % m_capacity;
        m_values[writeIndex] = value;
        ++m_size;
        return;
    }

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
    return values(m_size);
}

QVector<double> RingBuffer::values(int maxCount) const
{
    const int clampedCount = qBound(0, maxCount, m_size);
    // 从逻辑尾部回溯最近 N 个样本，调用方无需关心循环数组的物理布局。
    const int startOffset = m_size - clampedCount;

    QVector<double> result;
    result.reserve(clampedCount);

    for (int i = 0; i < clampedCount; ++i) {
        const int index = (m_head + startOffset + i) % m_capacity;
        result.append(m_values[index]);
    }

    return result;
}

bool RingBuffer::minMaxOfLast(int maxCount, double* minValue, double* maxValue) const
{
    if (!minValue || !maxValue || m_size == 0) {
        return false;
    }

    const int clampedCount = qBound(0, maxCount, m_size);
    if (clampedCount == 0) {
        return false;
    }

    // 直接扫描尾窗，避免为统计最值再额外构造一份 QVector。
    const int startOffset = m_size - clampedCount;
    double localMin = 0.0;
    double localMax = 0.0;

    for (int i = 0; i < clampedCount; ++i) {
        const int index = (m_head + startOffset + i) % m_capacity;
        const double value = m_values[index];

        if (i == 0 || value < localMin) {
            localMin = value;
        }

        if (i == 0 || value > localMax) {
            localMax = value;
        }
    }

    *minValue = localMin;
    *maxValue = localMax;
    return true;
}
