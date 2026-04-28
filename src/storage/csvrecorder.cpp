#include "csvrecorder.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>

CsvRecorder::CsvRecorder()
    : m_stream(&m_file)
    , m_channelCount(0)
    , m_headerWritten(false)
{
}

bool CsvRecorder::start(const QString& path)
{
    stop();

    const QFileInfo fileInfo(path);
    QDir dir = fileInfo.dir();
    if(!dir.exists() && !dir.mkpath(".")) {
        return false;
    }

    m_file.setFileName(path);
    if (!m_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    m_stream.setCodec("UTF-8");
    m_channelCount = 0;
    m_headerWritten = false;
    return true;
}

void CsvRecorder::append(const ProtocolFrame& frame)
{
    if(!isRecording() || !frame.isValid || frame.values.isEmpty()) {
        return;
    }

    if(!m_headerWritten) {
        m_channelCount = frame.values.size();
        writeHeader(m_channelCount);
        m_headerWritten = true;
    }

    const QString hostTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");

    m_stream << hostTime << ','
             << frame.sequence << ','
             << frame.timestampMs;

    for (int i = 0; i < m_channelCount; ++i) {
        if (i < frame.values.size()) {
            m_stream << ',' << frame.values[i];
        } else {
            m_stream << ',';
        }
    }

    m_stream << ','
             << escapeCsvField(QString::fromUtf8(frame.rawFrame))
             << '\n';
    m_stream.flush();
}

void CsvRecorder::stop()
{
    if (m_file.isOpen()) {
        m_stream.flush();
        m_file.close();
    }
    m_channelCount = 0;
    m_headerWritten = false;
}

bool CsvRecorder::isRecording() const
{
    return m_file.isOpen();
}

QString CsvRecorder::filePath() const
{
    return m_file.fileName();
}

void CsvRecorder::writeHeader(int channelCount)
{
    m_stream << "host_time,sequence,device_time_ms";
    for (int i = 0; i < channelCount; ++i) {
        m_stream << ",ch" << (i + 1);
    }
    m_stream << ",raw_frame\n";
    m_stream.flush();
}

QString CsvRecorder::escapeCsvField(const QString& text)
{
    QString escaped = text;
    escaped.replace('"', "\"\"");
    return QStringLiteral("\"%1\"").arg(escaped);
}
