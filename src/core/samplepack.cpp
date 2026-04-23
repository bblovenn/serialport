#include "samplepack.h"

SamplePack::SamplePack()
{
}

SamplePack::SamplePack(const QVector<double>& values)
    : m_values(values)
{
}

int SamplePack::channelCount() const
{
    return m_values.size();
}

double SamplePack::value(int channelIndex) const
{
    // 越界时返回 0.0，减少调用方的边界判断负担。
    return m_values.value(channelIndex, 0.0);
}

QVector<double> SamplePack::values() const
{
    return m_values;
}

bool SamplePack::isEmpty() const
{
    return m_values.isEmpty();
}
