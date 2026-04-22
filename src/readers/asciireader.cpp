#include "asciireader.h"

#include <QIODevice>
#include <QVector>

namespace {
constexpr int kMaxBufferSize = 4096;
}

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
    m_buffer.clear();
}

void AsciiReader::readAvailableData()
{
    if (!m_running) {
        return;
    }

    if (!m_device) {
        return;
    }

    const QByteArray chunk = m_device->readAll();
    m_buffer.append(chunk);

    if (m_buffer.size() > kMaxBufferSize && m_buffer.indexOf('\n') < 0) {
        m_buffer.clear();
        return;
    }

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
