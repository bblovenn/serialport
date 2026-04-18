#ifndef ABSTRACTREADER_H
#define ABSTRACTREADER_H

#include "core/samplepack.h"

#include <QObject>

class AbstractReader : public QObject
{
    Q_OBJECT
public:
    explicit AbstractReader(QObject *parent = nullptr);
    ~AbstractReader() override;

signals:

};

#endif // ABSTRACTREADER_H
