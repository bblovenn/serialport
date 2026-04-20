#ifndef ASCIIREADER_H
#define ASCIIREADER_H

#include "abstractreader.h"

#include <QByteArray>

class QIODevice;

class AsciiReader : public AbstractReader
{
    Q_OBJECT

public:
    explicit AsciiReader(QObject* parent = nullptr);

    void setDevice(QIODevice* device);

public slots:
    void start() override;
    void stop() override;

private slots:
    void readAvailableData();

private:
    void parseLine(const QByteArray& line);

    QIODevice* m_device;
    QByteArray m_buffer;
    bool m_running;
};

#endif // ASCIIREADER_H
