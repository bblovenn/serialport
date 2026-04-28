#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QVector>

class RingBuffer
{
public:
    explicit RingBuffer(int capacity = 1000);

    void setCapacity(int capacity);
    int capacity() const;
    int size() const;
    bool isEmpty() const;

    void append(double value);
    void clear();

    QVector<double> values() const;
    QVector<double> values(int maxCount) const;
    bool minMaxOfLast(int maxCount, double* minValue, double* maxValue) const;

private:
    int m_capacity;
    int m_head;
    int m_size;
    QVector<double> m_values;
};

#endif // RINGBUFFER_H
