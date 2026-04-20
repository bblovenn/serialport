#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QPointF>
#include <QTimer>
#include <QWidget>

class QPainter;
class QPaintEvent;
class Stream;

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
    void drawBackground(QPainter& painter);
    void drawGrid(QPainter& painter);
    void drawAxes(QPainter& painter);
    void drawCurves(QPainter& painter);
    void drawLabels(QPainter& painter);

    double resolveMinY() const;
    double resolveMaxY() const;
    QPointF mapSampleToPoint(int sampleIndex, int sampleCount, double value) const;

    Stream* m_stream;
    QTimer m_refreshTimer;
    int m_sampleWindow;
    bool m_autoScaleY;
    double m_minY;
    double m_maxY;
    bool m_paused;
};

#endif // PLOTWIDGET_H
