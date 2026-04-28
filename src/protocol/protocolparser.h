#ifndef PROTOCOLPARSER_H
#define PROTOCOLPARSER_H

#include "protocolframe.h"

#include <QByteArray>
#include <QVector>
#include <QtGlobal>

class ProtocolParser
{
public:
    ProtocolParser();

    void appendData(const QByteArray& data);
    QVector<ProtocolFrame> takeFrames();

    int parseErrorCount() const;
    int checksumErrorCount() const;
    int lostPacketCount() const;
    void reset();

private:
    void parseLine(const QByteArray& rawLine);
    static quint8 calculateChecksum(const QByteArray& payload);

    QByteArray m_buffer;
    QVector<ProtocolFrame> m_frames;
    int m_parseErrorCount;
    int m_checksumErrorCount;
    int m_lostPacketCount;
    bool m_hasLastSequence;
    quint32 m_lastSequence;
    quint64 m_lastTimestampMs;
};

#endif // PROTOCOLPARSER_H
