#ifndef SERIALCONTROLLER_H
#define SERIALCONTROLLER_H

#include <QObject>
#include <QStringList>

class QSerialPort;

class SerialController : public QObject
{
    Q_OBJECT

public:
    explicit SerialController(QObject* parent = nullptr);
    ~SerialController() override;

    QStringList availablePorts() const;
    QSerialPort* port() const;

    bool open(const QString& portName, int baudRate);
    void close();
    bool isOpen() const;

private:
    QSerialPort* m_port;
};

#endif // SERIALCONTROLLER_H
