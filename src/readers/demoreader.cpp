#include "demoreader.h"

#include <QtMath>

/*这是一个模拟数据生成器，用于测试/演示：

特性	说明
定时器	每 20ms 生成一次数据（约 50fps）
数据	2 通道：sin(phase) 和 cos(phase)
相位	递增 0.05，到 2π 时归零*/

// 构造函数，初始化相位为0.0，连接定时器信号
DemoReader::DemoReader(QObject* parent) : AbstractReader(parent), m_phase(0.0)
{
    // 定时器超时则调用 generateSample
    connect(&m_timer, &QTimer::timeout, this, &DemoReader::generateSample);
    m_timer.setInterval(20);  // 定时器间隔20ms（约50fps）
}

// 开始读取，启动定时器
void DemoReader::start()
{
    m_timer.start();
}

// 停止读取，停止定时器
void DemoReader::stop()
{
    m_timer.stop();
}

// 生成采样数据，每20ms调用一次
void DemoReader::generateSample()
{
    const double sinValue = qSin(m_phase);   // 正弦值
    const double cosValue = qCos(m_phase);    // 余弦值

    // 发出信号，包含2通道采样数据
    emit samplesReady(SamplePack(QVector<double>{sinValue, cosValue}));

    m_phase += 0.05;  // 相位递增

    // 相位归零，避免浮点数溢出
    if (m_phase > 2.0 * M_PI) {
        m_phase -= 2.0 * M_PI;
    }
}
