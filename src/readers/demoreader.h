#ifndef DEMOREADER_H
#define DEMOREADER_H

#include "abstractreader.h"

#include <QTimer>

class DemoReader : public AbstractReader
{

    Q_OBJECT

public:
    explicit DemoReader(QObject* parent = nullptr);

public slots:
    void start() override;//override重写基类的虚函数
    void stop() override;

private slots:
    void generateSample();

private :
    QTimer m_timer;
    double m_phase;

};

#endif // DEMOREADER_H
