#include "stream.h"

//Stream 多通道数据流管理器的实现。

/*核心功能
功能	说明
appendSamples()	接收 SamplePack 并分发数据到各通道
clear()	清空所有通道
setSampleWindow()	设置采样窗口，控制数据保留量
ensureChannelCount()	自动创建缺失的通道
数据流示意

数据源 (串口/传感器)
         ↓
    SamplePack
    { ch0:1.0, ch1:2.0, ch2:3.0 }
         ↓
   appendSamples(pack)
         ↓
  ┌──────────────────┐
  │ ensureChannelCount() │
  │ 自动创建 CH1/CH2/CH3 │
  └──────────────────┘
         ↓
分发到各 StreamChannel → RingBuffer 存储*/

// 多通道数据流管理器，负责：
// 接收批量采样数据并分发给各通道
// 管理通道生命周期
// 控制采样窗口和暂停状态

// 构造函数，初始化采样窗口1000，暂停状态为false
Stream::Stream(QObject *parent) : QObject(parent), m_sampleWindow(1000), m_paused(false)
{
}

// 析构函数，删除所有通道指针，防止内存泄漏
Stream::~Stream()
{
    qDeleteAll(m_channels);
}

// 追加一组采样数据到所有通道
void Stream::appendSamples(const SamplePack &pack)
{
    // 暂停或空数据直接返回
    if (m_paused || pack.isEmpty()) {
        return;
    }

    // 确保通道数量足够
    ensureChannelCount(pack.channelCount());

    // 分发数据到各通道
    for (int i = 0; i < pack.channelCount(); ++i) {
        m_channels[i]->append(pack.value(i));
    }
}

// 清空所有通道的数据
void Stream::clear()
{
    for (StreamChannel* streamChannel : m_channels) {
        streamChannel->clear();
    }
}

// 设置采样窗口大小，同时更新所有通道容量
void Stream::setSampleWindow(int samples)
{
    m_sampleWindow = qMax(1, samples);

    for (StreamChannel* streamChannel : m_channels) {
        streamChannel->setCapacity(m_sampleWindow);
    }
}

int Stream::sampleWindow() const
{
    return m_sampleWindow;
}

// 返回通道数量
int Stream::channelCount() const
{
    return m_channels.size();
}

// 获取指定通道，超界返回nullptr
StreamChannel* Stream::channel(int index)
{
    return m_channels.value(index, nullptr);
}

// const版本获取指定通道
const StreamChannel* Stream::channel(int index) const
{
    return m_channels.value(index, nullptr);
}

// 设置暂停状态
void Stream::setPaused(bool paused)
{
    m_paused = paused;
}

bool Stream::isPaused() const
{
    return m_paused;
}

// 确保通道数量足够，不足时自动创建
void Stream::ensureChannelCount(int count)
{
    while (m_channels.size() < count) {
        StreamChannel* streamChannel = new StreamChannel(m_sampleWindow);
        streamChannel->setName(QString("CH%1").arg(m_channels.size() + 1));
        m_channels.append(streamChannel);
    }
}