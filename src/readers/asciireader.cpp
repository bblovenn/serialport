#include "asciireader.h"

#include <QIODevice>
#include <QVector>

namespace {
// 防止设备持续发送无换行垃圾数据时，缓冲区无限增长。
constexpr int kMaxBufferSize = 4096;
constexpr char kProtocolPrefix[] = "$DATA,";
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
        // 切换设备前断开旧设备信号，避免旧串口继续触发读取。
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
    m_protocolParser.reset();
}

void AsciiReader::readAvailableData()
{
    if (!m_running || !m_device) {
        return;
    }

    const QByteArray chunk = m_device->readAll();
    if (chunk.isEmpty()) {
        return;
    }

    const QString rawText = QString::fromUtf8(chunk);
    if (!rawText.isEmpty()) {
        // 原始日志即时显示，保留空格和制表符，便于排查真实串口内容。
        emit rawLineReceived(rawText);
    }

    m_buffer.append(chunk);

    // 长时间收不到换行时，清理缓存，避免非法数据无限堆积。
    if (m_buffer.size() > kMaxBufferSize && m_buffer.indexOf('\n') < 0) {
        m_buffer.clear();
        return;
    }

    // 数值解析仍按完整行处理，防止半截数据被提前解析。
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
    if (line.startsWith(kProtocolPrefix)) {
        const int parseErrorsBefore = m_protocolParser.parseErrorCount();
        const int checksumErrorsBefore = m_protocolParser.checksumErrorCount();

        m_protocolParser.appendData(line + "\n");
        emitProtocolFrames();

        if (m_protocolParser.parseErrorCount() > parseErrorsBefore) {
            emit protocolParseErrorOccurred(QStringLiteral("协议帧解析失败：%1").arg(QString::fromUtf8(line)));
        } else if (m_protocolParser.checksumErrorCount() > checksumErrorsBefore) {
            emit protocolParseErrorOccurred(QStringLiteral("协议帧校验失败：%1").arg(QString::fromUtf8(line)));
        }
        return;
    }

    QVector<double> values;
    const QList<QByteArray> parts = line.split(',');

    for (const QByteArray& part : parts) {
        bool ok = false;
        const double value = part.trimmed().toDouble(&ok);
        if (!ok) {
            // 只要当前行有一个字段不是数字，就整行丢弃，避免通道错位。
            return;
        }

        values.append(value);
    }

    if (!values.isEmpty()) {
        emit samplesReady(SamplePack(values));
    }
}

void AsciiReader::emitProtocolFrames()
{
    const QVector<ProtocolFrame> frames = m_protocolParser.takeFrames();

    for (const ProtocolFrame& frame : frames) {
        if (!frame.isValid || frame.values.isEmpty()) {
            continue;
        }

        emit protocolFrameParsed(frame);
        emit samplesReady(SamplePack(frame.values));
    }
}
