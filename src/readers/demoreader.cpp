#include "demoreader.h"

#include <QtMath>

// DemoReader构造函数：初始化相位为0，创建定时器
DemoReader::DemoReader(QObject* parent)
    : AbstractReader(parent)
    , m_phase(0.0)  // 初始相位为0，sin/cos从0开始
{
    // 连接定时器超时信号到样本生成函数
    connect(&m_timer, &QTimer::timeout, this, &DemoReader::generateSample);
    m_timer.setInterval(20);  // 设置定时器间隔20ms，即50fps
}

// 启动数据生成：开始定时器
void DemoReader::start()
{
    m_timer.start();  // 定时器开始计时，每20ms触发一次generateSample
}

// 停止数据生成：停止定时器
void DemoReader::stop()
{
    m_timer.stop();  // 定时器停止，不再触发generateSample
}

// 生成一对演示样本：计算当前相位的sin值和cos值
// 每20ms调用一次，相位递增0.05（约2.86度）
// 完整周期 = 2π / 0.05 ≈ 126次 = 2.52秒
void DemoReader::generateSample()
{
    // 计算当前相位的正弦和余弦值，范围[-1, 1]
    const double sinValue = qSin(m_phase);  // CH1: 正弦波
    const double cosValue = qCos(m_phase);  // CH2: 余弦波

    // 发送双通道样本包（两列数据）
    emit samplesReady(SamplePack(QVector<double>{sinValue, cosValue}));

    // 相位递增0.05（约2.86度/次）
    m_phase += 0.05;

    // 当相位超过2π时归零，实现周期循环
    const double twoPi = 2.0 * 3.14159265358979323846;
    if (m_phase > twoPi) {
        m_phase -= twoPi;
    }
}
