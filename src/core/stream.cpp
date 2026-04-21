#include "stream.h"

#include <QColor>
#include <QtGlobal>

//多通道数据流管理器，负责接收、分发和存储采样数据。

// 构造函数，初始化采样窗口1000，暂停状态为false
Stream::Stream(QObject* parent)
    : QObject(parent)
    , m_sampleWindow(1000)  // 默认采样窗口1000点
    , m_paused(false)        // 默认非暂停状态
{
}

// 析构函数，删除所有通道指针，防止内存泄漏
Stream::~Stream()
{
    qDeleteAll(m_channels);  // 释放所有通道内存
}

// 追加一组采样数据到所有通道
void Stream::appendSamples(const SamplePack& pack)
{
    // 暂停或空数据直接返回
    if (m_paused || pack.isEmpty()) {
        return;
    }

    // 确保通道数量足够（根据SamplePack的通道数自动创建缺失通道）
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
    m_sampleWindow = qMax(1, samples);  // 确保至少为1

    // 更新所有已有通道的容量
    for (StreamChannel* streamChannel : m_channels) {
        streamChannel->setCapacity(m_sampleWindow);
    }
}

// 获取采样窗口大小
int Stream::sampleWindow() const
{
    return m_sampleWindow;
}

// 返回通道数量
int Stream::channelCount() const
{
    return m_channels.size();
}

// 获取指定通道（非const版本）
StreamChannel* Stream::channel(int index)
{
    return m_channels.value(index, nullptr);  // 超界返回nullptr
}

// 获取指定通道（const版本）
const StreamChannel* Stream::channel(int index) const
{
    return m_channels.value(index, nullptr);
}

// 设置暂停状态
void Stream::setPaused(bool paused)
{
    m_paused = paused;
}

// 获取暂停状态
bool Stream::isPaused() const
{
    return m_paused;
}

// 确保通道数量足够，不足时自动创建
void Stream::ensureChannelCount(int count)
{
    // 预设颜色数组，4种颜色循环使用
    const QVector<QColor> colors = {
        QColor(80, 220, 120),   // CH1: 绿色
        QColor(255, 214, 90),   // CH2: 黄色
        QColor(80, 170, 255),   // CH3: 蓝色
        QColor(255, 120, 150)   // CH4: 红色
    };

    // 循环创建直到数量足够
    while (m_channels.size() < count) {
        const int channelIndex = m_channels.size();  // 新通道的索引

        // 创建新通道，使用当前采样窗口作为容量
        StreamChannel* streamChannel = new StreamChannel(m_sampleWindow);
        // 自动命名：CH1, CH2, CH3...
        streamChannel->setName(QString("CH%1").arg(channelIndex + 1));
        // 分配颜色（循环使用）
        streamChannel->setColor(colors[channelIndex % colors.size()]);

        m_channels.append(streamChannel);  // 添加到通道列表
    }
}
