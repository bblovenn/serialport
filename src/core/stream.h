#ifndef STREAM_H
#define STREAM_H

#include <QObject>

class Stream : public QObject
{
    Q_OBJECT
public:
    explicit Stream(QObject *parent = nullptr);

signals:

};

#endif // STREAM_H
