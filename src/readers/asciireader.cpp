#include "asciireader.h"

#include <QIODevice>
#include <QVector>

namespace {
// 防止串口持续发送无换行垃圾数据时，缓冲区无限增长。
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
        // 原始日志即时显示，并保留空格、Tab 等字符，方便排查真实串口内容。
        emit rawLineReceived(rawText);
    }

    m_buffer.append(chunk);

    // 长时间收不到换行时，清理缓存，避免非法数据无限堆积。
    if (m_buffer.size() > kMaxBufferSize && m_buffer.indexOf('\n') < 0) {
        m_buffer.clear();
        return;
    }

    // 数值解析仍按完整行处理，防止 "0." 这类半截数据被提前解析。
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
            // 只要当前行有一个字段不是数字，就整行丢弃，避免通道错位。
            return;
        }

        values.append(value);
    }

    if (!values.isEmpty()) {
        // 一行数据对应一次 SamplePack，交给 Stream 按通道缓存。
        emit samplesReady(SamplePack(values));
    }
}
