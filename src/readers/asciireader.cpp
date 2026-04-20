#include "asciireader.h"

#include <QIODevice>
#include <QVector>

AsciiReader::AsciiReader(QObject* parent)
    : AbstractReader(parent)
    , m_device(nullptr)
    , m_running(false)
{
}

void AsciiReader::setDevice(QIODevice* device)
{
    if (m_device) {
        disconnect(m_device, nullptr, this, nullptr);
    }

    m_device = device;

    if (m_device) {
        connect(m_device, &QIODevice::readyRead, this, &AsciiReader::readAvailableData);
    }
}

void AsciiReader::start()
{
    m_running = true;
}

void AsciiReader::stop()
{
    m_running = false;
}

void AsciiReader::readAvailableData()
{
    if (!m_running || !m_device) {
        return;
    }

    m_buffer.append(m_device->readAll());

    int newlineIndex = m_buffer.indexOf('\n');
    while (newlineIndex >= 0) {
        const QByteArray line = m_buffer.left(newlineIndex).trimmed();
        m_buffer.remove(0, newlineIndex + 1);

        if (!line.isEmpty()) {
            parseLine(line);
        }

        newlineIndex = m_buffer.indexOf('\n');
    }
}

void AsciiReader::parseLine(const QByteArray& line)
{
    QVector<double> values;
    const QList<QByteArray> parts = line.split(',');

    for (const QByteArray& part : parts) {
        bool ok = false;
        const double value = part.trimmed().toDouble(&ok);
        if (!ok) {
            return;
        }

        values.append(value);
    }

    if (!values.isEmpty()) {
        emit samplesReady(SamplePack(values));
    }
}
