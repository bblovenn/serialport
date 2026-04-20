#include "serialcontroller.h"

#include <QIODevice>
#include <QSerialPort>
#include <QSerialPortInfo>

SerialController::SerialController(QObject* parent)
    : QObject(parent)
    , m_port(new QSerialPort(this))
{
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
        m_port->close();
    }

    m_port->setPortName(portName);
    m_port->setBaudRate(baudRate);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    return m_port->open(QIODevice::ReadWrite);
}

void SerialController::close()
{
    if (m_port->isOpen()) {
        m_port->close();
    }
}

bool SerialController::isOpen() const
{
    return m_port->isOpen();
}
