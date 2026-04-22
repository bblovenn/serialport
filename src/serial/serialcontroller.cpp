#include "serialcontroller.h"

#include <QIODevice>
#include <QSerialPort>
#include <QSerialPortInfo>

SerialController::SerialController(QObject* parent)
    : QObject(parent)
    , m_port(new QSerialPort(this))
    , m_lastErrorString()
{
    // 监听底层串口对象的错误信号，统一转到当前类中处理。
    connect(
        m_port,
        &QSerialPort::errorOccurred,
        this,
        &SerialController::handlePortError
    );
}

SerialController::~SerialController()
{
    close();
}

QStringList SerialController::availablePorts() const
{
    QStringList names;
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    // 扫描当前系统中可用的串口名称，供界面下拉框刷新使用。
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
    // 如果之前已经打开过串口，先关闭，避免复用旧状态。
    if (m_port->isOpen()) {
        m_port->close();
    }

    m_lastErrorString.clear();

    // 先把常用串口参数配置好，再执行真正的打开动作。
    m_port->setPortName(portName);
    m_port->setBaudRate(baudRate);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    if (!m_port->open(QIODevice::ReadWrite)) {
        // 打开失败时缓存错误信息，并通知上层界面显示原因。
        m_lastErrorString = m_port->errorString();
        emit serialErrorOccurred(m_lastErrorString);
        return false;
    }

    // 打开成功后发出信号，让主窗口更新按钮和运行状态。
    emit serialOpened(portName, baudRate);
    return true;
}

void SerialController::close()
{
    // 避免重复关闭导致无意义的状态切换。
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

QString SerialController::lastErrorString() const
{
    return m_lastErrorString;
}

void SerialController::handlePortError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) {
        return;
    }

    // 保留最近一次错误文本，便于界面提示或调试查看。
    m_lastErrorString = m_port->errorString();
    emit serialErrorOccurred(m_lastErrorString);

    // 这几类错误通常意味着串口已经不可继续使用，需要主动关闭并同步界面状态。
    if (error == QSerialPort::ResourceError ||
        error == QSerialPort::DeviceNotFoundError ||
        error == QSerialPort::PermissionError) {
        if (m_port->isOpen()) {
            m_port->close();
        }
        emit serialClosed();
    }
}
