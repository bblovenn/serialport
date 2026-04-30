#include "plotwidget.h"

#include "core/stream.h"

#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QVector>
#include <QtGlobal>

PlotWidget::PlotWidget(QWidget* parent)
    : QWidget(parent)
    , m_stream(nullptr)
    , m_sampleWindow(1000)
    , m_autoScaleY(false)
    , m_minY(-1.2)
    , m_maxY(1.2)
    , m_paused(false)
{
    setMinimumSize(600, 300);
    setAutoFillBackground(false);

    connect(&m_refreshTimer, &QTimer::timeout, this, QOverload<>::of(&PlotWidget::update));
    m_refreshTimer.start(33);
}

void PlotWidget::setStream(Stream* stream)
{
    m_stream = stream;
    update();
}

void PlotWidget::setSampleWindow(int samples)
{
    m_sampleWindow = qMax(1, samples);
    update();
}

void PlotWidget::setAutoScaleY(bool enabled)
{
    m_autoScaleY = enabled;
    update();
}

void PlotWidget::setYAxisRange(double min, double max)
{
    if (qFuzzyCompare(min, max)) {
        return;
    }

    m_minY = qMin(min, max);
    m_maxY = qMax(min, max);
    update();
}

QPair<double, double> PlotWidget::currentYAxisRange() const
{
    double minY = resolveMinY();
    double maxY = resolveMaxY();

    if (qFuzzyCompare(minY, maxY)) {
        minY -= 1.0;
        maxY += 1.0;
    }

    return qMakePair(minY, maxY);
}

void PlotWidget::clear()
{
    if (m_stream) {
        m_stream->clear();
    }
    update();
}

void PlotWidget::setPaused(bool paused)
{
    m_paused = paused;

    if (m_stream) {
        m_stream->setPaused(paused);
    }

    update();
}

void PlotWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    drawBackground(painter);
    drawGrid(painter);
    drawAxes(painter);
    drawCurves(painter);
    drawLabels(painter);
}

void PlotWidget::drawBackground(QPainter& painter)
{
    const QRectF panelRect = rect().adjusted(1, 1, -2, -2);

    painter.setPen(QPen(QColor(42, 52, 62), 1));
    painter.setBrush(QColor(12, 17, 22));
    painter.drawRoundedRect(panelRect, 12, 12);
}

void PlotWidget::drawGrid(QPainter& painter)
{
    const QRect plotRect = rect().adjusted(60, 20, -20, -48);

    painter.setPen(QPen(QColor(38, 48, 58), 1));

    const int verticalLines = 10;
    for (int i = 0; i <= verticalLines; ++i) {
        const double ratio = static_cast<double>(i) / verticalLines;
        const int x = plotRect.left() + static_cast<int>(ratio * plotRect.width());
        painter.drawLine(x, plotRect.top(), x, plotRect.bottom());
    }

    const int horizontalLines = 6;
    for (int i = 0; i <= horizontalLines; ++i) {
        const double ratio = static_cast<double>(i) / horizontalLines;
        const int y = plotRect.top() + static_cast<int>(ratio * plotRect.height());
        painter.drawLine(plotRect.left(), y, plotRect.right(), y);
    }
}

void PlotWidget::drawAxes(QPainter& painter)
{
    const QRect plotRect = rect().adjusted(60, 20, -20, -48);

    painter.setPen(QPen(QColor(92, 106, 120), 1));
    painter.drawLine(plotRect.left(), plotRect.bottom(), plotRect.right(), plotRect.bottom());
    painter.drawLine(plotRect.left(), plotRect.top(), plotRect.left(), plotRect.bottom());

    const QPair<double, double> range = currentYAxisRange();
    const double minY = range.first;
    const double maxY = range.second;
    const int horizontalTicks = 6;
    const int verticalTicks = 10;

    painter.setPen(QColor(204, 213, 222));
    QFontMetrics metrics = painter.fontMetrics();

    for (int i = 0; i <= horizontalTicks; ++i) {
        const double ratio = static_cast<double>(i) / horizontalTicks;
        const int y = plotRect.top() + static_cast<int>(ratio * plotRect.height());
        const double value = maxY - ratio * (maxY - minY);
        const QString label = QString::number(value, 'f', 2);
        painter.drawText(8, y + metrics.ascent() / 2, label);
    }

    for (int i = 0; i <= verticalTicks; ++i) {
        const double ratio = static_cast<double>(i) / verticalTicks;
        const int x = plotRect.left() + static_cast<int>(ratio * plotRect.width());
        const int sampleValue = static_cast<int>(ratio * m_sampleWindow);
        const QString label = QString::number(sampleValue);
        const int textWidth = metrics.horizontalAdvance(label);
        painter.drawText(x - textWidth / 2, plotRect.bottom() + metrics.height() + 6, label);
    }
}

void PlotWidget::drawCurves(QPainter& painter)
{
    if (!m_stream || m_stream->channelCount() == 0) {
        return;
    }

    const QVector<QColor> colors = {
        QColor(74, 222, 128),
        QColor(250, 204, 21),
        QColor(56, 189, 248),
        QColor(251, 113, 133)
    };

    const QPair<double, double> range = currentYAxisRange();
    const double minY = range.first;
    const double maxY = range.second;

    for (int channelIndex = 0; channelIndex < m_stream->channelCount(); ++channelIndex) {
        const StreamChannel* channel = m_stream->channel(channelIndex);
        if (!channel || !channel->isVisible()) {
            continue;
        }

        // 只取当前可见窗口，避免每帧复制整段历史数据。
        const QVector<double> values = channel->values(m_sampleWindow);
        if (values.size() < 2) {
            continue;
        }

        QPainterPath path;
        path.moveTo(mapSampleToPoint(0, values.size(), values[0], minY, maxY));

        for (int i = 1; i < values.size(); ++i) {
            path.lineTo(mapSampleToPoint(i, values.size(), values[i], minY, maxY));
        }

        painter.setPen(QPen(colors[channelIndex % colors.size()], 2.2));
        painter.drawPath(path);
    }
}

void PlotWidget::drawLabels(QPainter& painter)
{
    const QRect plotRect = rect().adjusted(60, 20, -20, -48);

    painter.setPen(QColor(219, 226, 233));
    painter.drawText(12, height() - 12, QString("Window: %1").arg(m_sampleWindow));

    if (m_paused) {
        painter.setPen(QColor(255, 214, 90));
        painter.drawText(plotRect.right() - 60, height() - 12, "Paused");
    }

    if (!m_stream) {
        painter.setPen(QColor(180, 184, 188));
        painter.drawText(plotRect.center(), "No stream");
        return;
    }

    int labelX = plotRect.left();
    const int labelY = plotRect.top() - 6;

    for (int i = 0; i < m_stream->channelCount(); ++i) {
        const StreamChannel* channel = m_stream->channel(i);
        if (!channel) {
            continue;
        }

        const QString name = channel->name().isEmpty()
            ? QString("CH%1").arg(i + 1)
            : channel->name();

        painter.setPen(channel->color().isValid() ? channel->color() : QColor(210, 214, 218));
        painter.drawText(labelX, labelY, name);
        labelX += 48;
    }
}

double PlotWidget::resolveMinY() const
{
    if (!m_autoScaleY || !m_stream) {
        return m_minY;
    }

    bool hasValue = false;
    double minValue = 0.0;

    for (int channelIndex = 0; channelIndex < m_stream->channelCount(); ++channelIndex) {
        const StreamChannel* channel = m_stream->channel(channelIndex);
        if (!channel || !channel->isVisible()) {
            continue;
        }

        double channelMin = 0.0;
        double channelMax = 0.0;
        // 自动缩放只看当前窗口，避免旧极值长期影响实时显示。
        if (!channel->minMaxOfLast(m_sampleWindow, &channelMin, &channelMax)) {
            continue;
        }

        Q_UNUSED(channelMax)
        if (!hasValue || channelMin < minValue) {
            minValue = channelMin;
            hasValue = true;
        }
    }

    return hasValue ? minValue : m_minY;
}

double PlotWidget::resolveMaxY() const
{
    if (!m_autoScaleY || !m_stream) {
        return m_maxY;
    }

    bool hasValue = false;
    double maxValue = 0.0;

    for (int channelIndex = 0; channelIndex < m_stream->channelCount(); ++channelIndex) {
        const StreamChannel* channel = m_stream->channel(channelIndex);
        if (!channel || !channel->isVisible()) {
            continue;
        }

        double channelMin = 0.0;
        double channelMax = 0.0;
        if (!channel->minMaxOfLast(m_sampleWindow, &channelMin, &channelMax)) {
            continue;
        }

        Q_UNUSED(channelMin)
        if (!hasValue || channelMax > maxValue) {
            maxValue = channelMax;
            hasValue = true;
        }
    }

    return hasValue ? maxValue : m_maxY;
}

QPointF PlotWidget::mapSampleToPoint(
    int sampleIndex,
    int sampleCount,
    double value,
    double minY,
    double maxY
) const
{
    const QRect plotRect = rect().adjusted(60, 20, -20, -48);

    if (sampleCount <= 1) {
        return QPointF(plotRect.left(), plotRect.center().y());
    }

    const double xRatio = static_cast<double>(sampleIndex) / static_cast<double>(sampleCount - 1);
    const double yRatio = (value - minY) / (maxY - minY);

    const double x = plotRect.left() + xRatio * plotRect.width();
    const double y = plotRect.bottom() - yRatio * plotRect.height();
    return QPointF(x, y);
}
