#include "samplepack.h"

//封装一组采样数据
//一个数据容器类，通常用于：

//串口/传感器数据采集
//多通道采样数据打包
//数据封装传递，便于扩展和维护

SamplePack::SamplePack()
{

}

// 从向量构造，使用初始化列表直接初始化成员
SamplePack::SamplePack(const QVector<double> &values)
    : m_values(values)
{

}

// 返回通道数量，直接返回 QVector 的大小
int SamplePack::channelCount() const
{
    return m_values.size();
}

// 获取指定通道的值，使用 QVector 的 value 方法提供默认值 0.0
double SamplePack::value(int channelIndex) const
{
    return m_values.value(channelIndex,0.0);
}

// 返回所有通道的值，直接返回 QVector 对象
QVector<double> SamplePack::values() const
{
    return m_values;
}

bool SamplePack::isEmpty() const
{
    return m_values.isEmpty();
}
