#ifndef ASCIIREADER_H
#define ASCIIREADER_H

#include "abstractreader.h"
#include "protocol/protocolparser.h"

#include <QByteArray>
#include <QString>

class QIODevice;

// AsciiReader 负责把 QIODevice 中的文本解析成采样数据或协议帧。
// 它兼容两种输入：
// 1. 普通 ASCII 数值行，例如 `0.1,0.2,0.3`
// 2. 设备协议帧，例如 `$DATA,...*CRC`
class AsciiReader : public AbstractReader
{
    Q_OBJECT

public:
    explicit AsciiReader(QObject* parent = nullptr);

    void setDevice(QIODevice* device);

public slots:
    void start() override;
    void stop() override;

signals:
    // 原始文本直接给日志面板显示，不要求一定是完整数值行。
    void rawLineReceived(const QString& text);
    void protocolFrameParsed(const ProtocolFrame& frame);
    void protocolParseErrorOccurred(const QString& message);

private slots:
    void readAvailableData();

private:
    void parseLine(const QByteArray& line);
    void emitProtocolFrames();

    QIODevice* m_device;         // 可以是真实串口，也可以是测试用设备。
    QByteArray m_buffer;         // 缓存尚未拼成完整行的数据。
    ProtocolParser m_protocolParser;
    bool m_running;
};

#endif // ASCIIREADER_H
