#ifndef ABSTRACTREADER_H
#define ABSTRACTREADER_H

#include <QObject>

class AbstractReader : public QObject
{
    Q_OBJECT
public:
    explicit AbstractReader(QObject *parent = nullptr);

signals:

};

#endif // ABSTRACTREADER_H
