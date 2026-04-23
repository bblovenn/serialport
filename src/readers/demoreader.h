#ifndef DEMOREADER_H
#define DEMOREADER_H

#include "abstractreader.h"

#include <QTimer>

// DemoReader 用定时器生成两路正弦/余弦示例数据。
// 这个类主要用于没有真实串口输入时的演示或调试；当前主界面默认走串口读取。
class DemoReader : public AbstractReader
{
    Q_OBJECT

public:
    explicit DemoReader(QObject* parent = nullptr);

public slots:
    void start() override;
    void stop() override;

private slots:
    // 定时产生一帧模拟采样，并通过 samplesReady 发出。
    void generateSample();

private:
    QTimer m_timer; // 控制模拟数据产生频率。
    double m_phase; // 当前波形相位。
};

#endif // DEMOREADER_H
