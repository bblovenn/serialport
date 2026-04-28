#ifndef PROTOCOLFRAME_H
#define PROTOCOLFRAME_H

#include <QByteArray>
#include <QVector>
#include <QtGlobal>

class ProtocolFrame
{
public:
    ProtocolFrame();

    bool isValid;
    quint32 sequence;
    quint64 timestampMs;
    QVector<double> values;
    QByteArray rawFrame;
};

#endif // PROTOCOLFRAME_H
