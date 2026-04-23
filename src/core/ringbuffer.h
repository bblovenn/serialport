#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <QVector>

// RingBuffer 是固定容量环形缓冲区，用于保存单个通道最近 N 个采样值。
// 新数据持续追加；容量满后覆盖最旧的数据，避免波形运行时间变长后内存无限增长。
// values() 会按“旧 -> 新”的顺序导出，绘图层可以直接按顺序连线。
class RingBuffer
{
public:
    explicit RingBuffer(int capacity = 1000);

    // 调整容量时尽量保留最新的数据点。
    void setCapacity(int capacity);
    int capacity() const;
    int size() const;
    bool isEmpty() const;

    void append(double value);
    void clear();

    // 导出当前窗口内的有序数据，隐藏内部环形索引细节。
    QVector<double> values() const;

private:
    int m_capacity;        // 缓冲区最多保存的数据点数量。
    int m_head;            // 当前最旧数据所在的位置。
    int m_size;            // 当前有效数据点数量。
    QVector<double> m_values; // 固定长度底层存储。
};

#endif // RINGBUFFER_H
