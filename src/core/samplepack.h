#ifndef SAMPLEPACK_H
#define SAMPLEPACK_H

#include <QVector>


class samplepack
{
public:
    samplepack();
    explicit samplepack(const QVector<double>& values); // 从向量构造，explicit防止隐式转换

    int channelCount() const; // 返回通道数量
    double value(int channelIndex) const;
    QVector<double> values() const;
    bool isEmpty() const;

private:
    QVector<double> m_values;
};

#endif // SAMPLEPACK_H
