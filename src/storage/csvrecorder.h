#ifndef CSVRECORDER_H
#define CSVRECORDER_H

#include "protocol/protocolframe.h"

#include <QFile>
#include <QTextStream>
#include <QString>

class CsvRecorder
{
public:
    CsvRecorder();
    
    bool start(const QString& path);
    void append(const ProtocolFrame& frame);
    void stop();

    bool isRecording() const;
    QString filePath() const;

private:
    void writeHeader(int channelCount);
    static QString escapeCsvField(const QString& text);

    QFile m_file;
    QTextStream m_stream;
    int m_channelCount;
    bool m_headerWritten;
    
};


#endif // CSVRECORDER_H