#include "protocolparser.h"

#include <QStringList>

ProtocolParser::ProtocolParser()
    : m_parseErrorCount(0)
    , m_checksumErrorCount(0)
    , m_lostPacketCount(0)
    , m_hasLastSequence(false)
    , m_lastSequence(0)
    , m_lastTimestampMs(0)
{
}

void ProtocolParser::appendData(const QByteArray& data)
{
    if (data.isEmpty()) {
        return;
    }

    // 串口数据可能分多次到达，先累计，等拿到完整换行后再按帧解析。
    m_buffer.append(data);

    int newlineIndex = m_buffer.indexOf('\n');
    while (newlineIndex >= 0) {
        const QByteArray rawLine = m_buffer.left(newlineIndex).trimmed();
        m_buffer.remove(0, newlineIndex + 1);

        if (!rawLine.isEmpty()) {
            parseLine(rawLine);
        }

        newlineIndex = m_buffer.indexOf('\n');
    }
}

QVector<ProtocolFrame> ProtocolParser::takeFrames()
{
    const QVector<ProtocolFrame> frames = m_frames;
    m_frames.clear();
    return frames;
}

int ProtocolParser::parseErrorCount() const
{
    return m_parseErrorCount;
}

int ProtocolParser::checksumErrorCount() const
{
    return m_checksumErrorCount;
}

int ProtocolParser::lostPacketCount() const
{
    return m_lostPacketCount;
}

void ProtocolParser::reset()
{
    m_buffer.clear();
    m_frames.clear();
    m_parseErrorCount = 0;
    m_checksumErrorCount = 0;
    m_lostPacketCount = 0;
    m_hasLastSequence = false;
    m_lastSequence = 0;
    m_lastTimestampMs = 0;
}

void ProtocolParser::parseLine(const QByteArray& rawLine)
{
    if (!rawLine.startsWith("$")) {
        ++m_parseErrorCount;
        return;
    }

    const int checksumSeparatorIndex = rawLine.lastIndexOf('*');
    if (checksumSeparatorIndex <= 1 || checksumSeparatorIndex >= rawLine.size() - 1) {
        ++m_parseErrorCount;
        return;
    }

    const QByteArray payload = rawLine.mid(1, checksumSeparatorIndex - 1);
    bool checksumOk = false;
    const int expectedChecksum = rawLine.mid(checksumSeparatorIndex + 1).toInt(&checksumOk, 16);
    if (!checksumOk) {
        ++m_parseErrorCount;
        return;
    }

    if (calculateChecksum(payload) != static_cast<quint8>(expectedChecksum)) {
        ++m_checksumErrorCount;
        return;
    }

    const QStringList fields = QString::fromUtf8(payload).split(',');
    if (fields.size() < 4 || fields.first() != QStringLiteral("DATA")) {
        ++m_parseErrorCount;
        return;
    }

    bool sequenceOk = false;
    bool timestampOk = false;
    const quint32 sequence = fields[1].toUInt(&sequenceOk);
    const quint64 timestampMs = fields[2].toULongLong(&timestampOk);
    if (!sequenceOk || !timestampOk) {
        ++m_parseErrorCount;
        return;
    }

    QVector<double> values;
    for (int i = 3; i < fields.size(); ++i) {
        bool valueOk = false;
        const double value = fields[i].toDouble(&valueOk);
        if (!valueOk) {
            ++m_parseErrorCount;
            return;
        }
        values.append(value);
    }

    if (values.isEmpty()) {
        ++m_parseErrorCount;
        return;
    }

    if (!m_hasLastSequence) {
        // 第一帧只建立统计基线，不参与丢包计算。
        m_hasLastSequence = true;
        m_lastSequence = sequence;
        m_lastTimestampMs = timestampMs;
    } else if (sequence == 0 && m_lastSequence != 0) {
        // 设备重新从 0 起号时，同步重建丢包统计基线。
        m_lastSequence = sequence;
        m_lastTimestampMs = timestampMs;
    } else if (sequence > m_lastSequence) {
        const quint32 expectedSequence = m_lastSequence + 1;
        if (sequence > expectedSequence) {
            m_lostPacketCount += static_cast<int>(sequence - expectedSequence);
        }

        m_lastSequence = sequence;
        m_lastTimestampMs = timestampMs;
    } else if (timestampMs < m_lastTimestampMs) {
        // 设备时间戳回退通常意味着重启，此时重新建立丢包统计基线。
        m_lastSequence = sequence;
        m_lastTimestampMs = timestampMs;
    }

    ProtocolFrame frame;
    frame.isValid = true;
    frame.sequence = sequence;
    frame.timestampMs = timestampMs;
    frame.values = values;
    frame.rawFrame = rawLine;
    m_frames.append(frame);
}

quint8 ProtocolParser::calculateChecksum(const QByteArray& payload)
{
    quint8 checksum = 0;
    for (char byte : payload) {
        checksum ^= static_cast<quint8>(byte);
    }
    return checksum;
}
