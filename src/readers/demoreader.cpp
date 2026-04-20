#include "demoreader.h"

#include <QtMath>

DemoReader::DemoReader(QObject* parent)
    : AbstractReader(parent)
    , m_phase(0.0)
{
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
    const double sinValue = qSin(m_phase);
    const double cosValue = qCos(m_phase);

    emit samplesReady(SamplePack(QVector<double>{sinValue, cosValue}));

    m_phase += 0.05;

    const double twoPi = 2.0 * 3.14159265358979323846;
    if (m_phase > twoPi) {
        m_phase -= twoPi;
    }
}
