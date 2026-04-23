#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QPointF>
#include <QTimer>
#include <QWidget>

class QPainter;
class QPaintEvent;
class Stream;

// PlotWidget 负责把 Stream 中的多通道数据绘制为波形。
// 它只做显示，不负责串口读取和数据解析；数据入口由 setStream() 指定。
// 刷新由内部定时器控制，避免每收到一个采样点就立即重绘导致界面卡顿。
class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget(QWidget* parent = nullptr);

    void setStream(Stream* stream);
    void setSampleWindow(int samples);
    void setAutoScaleY(bool enabled);
    void setYAxisRange(double min, double max);

public slots:
    void clear();
    void setPaused(bool paused);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    // 绘制流程按背景、网格、坐标轴、曲线、标签分层，便于单独调整视觉效果。
    void drawBackground(QPainter& painter);
    void drawGrid(QPainter& painter);
    void drawAxes(QPainter& painter);
    void drawCurves(QPainter& painter);
    void drawLabels(QPainter& painter);

    double resolveMinY() const;
    double resolveMaxY() const;
    QPointF mapSampleToPoint(
        int sampleIndex,
        int sampleCount,
        double value,
        double minY,
        double maxY
    ) const;

    Stream* m_stream;          // 当前绑定的数据流，不由 PlotWidget 释放。
    QTimer m_refreshTimer;     // 定时刷新，避免每个采样点都触发重绘。
    int m_sampleWindow;        // 横向最多显示的采样点数量。
    bool m_autoScaleY;         // 是否根据数据自动计算 Y 轴范围。
    double m_minY;             // 手动 Y 轴最小值。
    double m_maxY;             // 手动 Y 轴最大值。
    bool m_paused;             // 暂停时绘制暂停提示。
};

#endif // PLOTWIDGET_H
