#ifndef ASCIIREADER_H
#define ASCIIREADER_H

#include "abstractreader.h"

#include <QByteArray>
#include <QString>

class QIODevice;

// AsciiReader 负责把 QIODevice 中的 ASCII 文本解析成 SamplePack。
// 原始文本收到后会立即发给日志面板；数值解析仍按换行分帧，避免半截数字被误画到波形里。
// 支持格式示例：
//   单通道：0.5\n
//   多通道：0.1,0.2,0.3\n
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
    // 供日志面板显示原始接收文本，不要求一定是完整数值行。
    void rawLineReceived(const QString& text);

private slots:
    void readAvailableData();

private:
    void parseLine(const QByteArray& line);

    QIODevice* m_device;  // 可以是 QSerialPort，也可以是测试用的其他 QIODevice。
    QByteArray m_buffer;  // 保存尚未凑成完整一行的残余数据。
    bool m_running;
};

#endif // ASCIIREADER_H
