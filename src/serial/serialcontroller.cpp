#include "serialcontroller.h"

#include <QIODevice>
#include <QSerialPort>
#include <QSerialPortInfo>

SerialController::SerialController(QObject* parent)
    : QObject(parent)
    , m_port(new QSerialPort(this))
    , m_lastErrorString()
{
    // 底层串口一旦报错，统一由当前类向上转发。
    connect(m_port, &QSerialPort::errorOccurred, this, &SerialController::handlePortError);
}

SerialController::~SerialController()
{
    close();
}

QStringList SerialController::availablePorts() const
{
    QStringList names;
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo& info : ports) {
        names.append(info.portName());
    }

    return names;
}

QSerialPort* SerialController::port() const
{
    return m_port;
}

bool SerialController::open(const QString& portName, int baudRate)
{
    if (m_port->isOpen()) {
        // 重新打开前先关闭旧连接，避免端口配置残留。
        m_port->close();
    }

    m_lastErrorString.clear();

    m_port->setPortName(portName);
    m_port->setBaudRate(baudRate);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    // 打开失败时保留错误文本，并通过信号交给界面层提示用户。
    if (!m_port->open(QIODevice::ReadWrite)) {
        m_lastErrorString = m_port->errorString();
        emit serialErrorOccurred(m_lastErrorString);
        return false;
    }

    emit serialOpened(portName, baudRate);
    return true;
}

void SerialController::close()
{
    if (!m_port->isOpen()) {
        return;
    }

    m_port->close();
    emit serialClosed();
}

bool SerialController::isOpen() const
{
    return m_port->isOpen();
}

bool SerialController::send(const QByteArray& data)
{
    if (data.isEmpty()) {
        m_lastErrorString = QStringLiteral("发送内容为空");
        return false;
    }

    if (!m_port || !m_port->isOpen()) {
        m_lastErrorString = QStringLiteral("串口未打开");
        return false;
    }

    qint64 totalWritten = 0;
    while (totalWritten < data.size()) {
        // write() 可能只接收部分数据，必须继续写剩余内容，避免命令被截断。
        const qint64 written = m_port->write(
            data.constData() + totalWritten,
            data.size() - totalWritten
        );

        if (written < 0) {
            m_lastErrorString = m_port->errorString();
            emit serialErrorOccurred(m_lastErrorString);
            return false;
        }

        if (written == 0) {
            if (!m_port->waitForBytesWritten(100)) {
                m_lastErrorString = m_port->errorString();
                if (m_lastErrorString.isEmpty()) {
                    m_lastErrorString = QStringLiteral("串口暂时无法写入");
                }
                return false;
            }
            continue;
        }

        totalWritten += written;
    }

    // flush() 只是尝试立刻把 Qt 缓冲区推到底层系统；返回 false 不代表发送失败。
    m_port->flush();
    m_lastErrorString.clear();
    return true;
}

QString SerialController::lastErrorString() const
{
    return m_lastErrorString;
}

void SerialController::handlePortError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) {
        return;
    }

    m_lastErrorString = m_port->errorString();
    emit serialErrorOccurred(m_lastErrorString);

    // 这几类错误通常意味着串口已经不可继续使用，需要同步关闭状态。
    if (error == QSerialPort::ResourceError ||
        error == QSerialPort::DeviceNotFoundError ||
        error == QSerialPort::PermissionError) {
        if (m_port->isOpen()) {
            m_port->close();
        }
        emit serialClosed();
    }
}
