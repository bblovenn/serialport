#include "demoreader.h"

#include <QtMath>

DemoReader::DemoReader(QObject* parent)
    : AbstractReader(parent)
    , m_phase(0.0)
{
    // 20ms 约等于 50FPS，适合观察绘图刷新是否流畅。
    connect(&m_timer, &QTimer::timeout, this, &DemoReader::generateSample);
    m_timer.setInterval(20);
}

void DemoReader::start()
{
    m_timer.start();
}

void DemoReader::stop()
{
    m_timer.stop();
}

void DemoReader::generateSample()
{
    // 输出两路标准波形，方便观察绘图和暂停功能是否正常。
    const double sinValue = qSin(m_phase);
    const double cosValue = qCos(m_phase);

    emit samplesReady(SamplePack(QVector<double>{sinValue, cosValue}));

    m_phase += 0.05;

    // 相位限制在 0~2π，避免长时间运行后浮点数持续变大。
    const double twoPi = 2.0 * 3.14159265358979323846;
    if (m_phase > twoPi) {
        m_phase -= twoPi;
    }
}
