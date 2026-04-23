#ifndef SERIALCONTROLLER_H
#define SERIALCONTROLLER_H

#include <QObject>
#include <QByteArray>
#include <QSerialPort>
#include <QString>
#include <QStringList>

// SerialController 负责串口的打开、关闭、发送和错误转发。
// MainWindow 通过它统一访问 QSerialPort，避免界面层散落底层串口配置代码。
// AsciiReader 直接读取 port() 暴露的 QSerialPort，但打开/关闭/发送仍由本类统一管理。
class SerialController : public QObject
{
    Q_OBJECT

public:
    explicit SerialController(QObject* parent = nullptr);
    ~SerialController() override;

    QStringList availablePorts() const; // 扫描当前系统可用串口名。
    QSerialPort* port() const;          // 暴露给 AsciiReader 作为 QIODevice 读取。

    bool open(const QString& portName, int baudRate);
    void close();
    bool isOpen() const;
    bool send(const QByteArray& data);
    QString lastErrorString() const;

signals:
    // 这些信号让 MainWindow 只关心业务状态，不直接依赖 QSerialPort 错误枚举。
    void serialOpened(const QString& portName, int baudRate);
    void serialClosed();
    void serialErrorOccurred(const QString& message);

private slots:
    void handlePortError(QSerialPort::SerialPortError error);

private:
    QSerialPort* m_port;       // Qt 串口对象，生命周期由 SerialController 管理。
    QString m_lastErrorString; // 最近一次失败原因，供界面提示。
};

#endif // SERIALCONTROLLER_H
