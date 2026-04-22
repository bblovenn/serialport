#ifndef SERIALCONTROLLER_H
#define SERIALCONTROLLER_H

#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QStringList>

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

    QString lastErrorString() const;

signals:
    void serialOpened(const QString& portName, int baudRate);
    void serialClosed();
    void serialErrorOccurred(const QString& message);

private slots:
    void handlePortError(QSerialPort::SerialPortError error);

private:
    QSerialPort* m_port;
    QString m_lastErrorString;
};

#endif // SERIALCONTROLLER_H
