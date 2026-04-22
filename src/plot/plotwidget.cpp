#include "plotwidget.h"

#include "core/stream.h"

#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QPen>
#include <QVector>
#include <QtGlobal>

// PlotWidget构造函数：初始化绘图参数，创建刷新定时器（约30fps）
PlotWidget::PlotWidget(QWidget* parent)
    : QWidget(parent)
    , m_stream(nullptr)
    , m_sampleWindow(1000)   // 默认采样窗口1000点
    , m_autoScaleY(false)    // 默认关闭Y轴自动缩放
    , m_minY(-1.2)           // 默认Y轴最小值
    , m_maxY(1.2)            // 默认Y轴最大值
    , m_paused(false)        // 默认非暂停
{
    setMinimumSize(600, 300);           // 设置最小尺寸
    setAutoFillBackground(false);        // 不自动填充背景（便于自定义绘制）

    // 约 30 FPS 刷新，兼顾曲线流畅度和重绘开销。
    connect(&m_refreshTimer, &QTimer::timeout, this, QOverload<>::of(&PlotWidget::update));
    m_refreshTimer.start(33);
}

// 关联数据流：设置要绑定的Stream指针
void PlotWidget::setStream(Stream* stream)
{
    m_stream = stream;
    update();  // 立即刷新
}

// 设置采样窗口：控制显示的数据点数量
void PlotWidget::setSampleWindow(int samples)
{
    m_sampleWindow = qMax(1, samples);  // 至少为1
    update();
}

// 设置Y轴自动缩放：启用时自动调整Y轴范围以适应数据
void PlotWidget::setAutoScaleY(bool enabled)
{
    m_autoScaleY = enabled;
    update();
}

// 设置Y轴固定范围：min和max会自动取较小/较大值
void PlotWidget::setYAxisRange(double min, double max)
{
    if (qFuzzyCompare(min, max)) {  // 防止min==max导致除零
        return;
    }

    m_minY = qMin(min, max);  // 确保m_minY < m_maxY
    m_maxY = qMax(min, max);
    update();
}

// 清空绘图：同时清空数据流和触发重绘
void PlotWidget::clear()
{
    if (m_stream) {
        m_stream->clear();  // 清空数据流中的所有通道
    }
    update();
}

// 设置暂停状态：同时暂停数据流
void PlotWidget::setPaused(bool paused)
{
    m_paused = paused;

    if (m_stream) {
        m_stream->setPaused(paused);  // 同步暂停数据流
    }

    update();
}

// paintEvent：Qt自动调用，触发一帧的绘制
void PlotWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);  // 启用抗锯齿

    drawBackground(painter);   // 绘制深色背景
    drawGrid(painter);         // 绘制网格线
    drawAxes(painter);         // 绘制坐标轴和刻度标签
    drawCurves(painter);       // 绘制波形曲线
    drawLabels(painter);       // 绘制通道标签和状态信息
}

// 绘制深色背景和边框
void PlotWidget::drawBackground(QPainter& painter)
{
    painter.fillRect(rect(), QColor(18, 20, 22));         // 深灰黑色背景
    painter.setPen(QPen(QColor(80, 84, 88), 1));          // 边框灰色
    painter.drawRect(rect().adjusted(0, 0, -1, -1));      // 绘制边框（向内缩进1像素避免超出）
}

// 绘制网格线：垂直10等分，水平8等分
void PlotWidget::drawGrid(QPainter& painter)
{
    const QRect plotRect = rect().adjusted(48, 20, -20, -36);  // 绘图区域（左侧留48像素给Y轴标签，右侧20px，顶部20px，底部36px）
    painter.setPen(QPen(QColor(52, 56, 60), 1));               // 网格线颜色

    const int verticalLines = 10;
    for (int i = 0; i <= verticalLines; ++i) {
        const double ratio = static_cast<double>(i) / verticalLines;
        const int x = plotRect.left() + static_cast<int>(ratio * plotRect.width());
        painter.drawLine(x, plotRect.top(), x, plotRect.bottom());
    }

    const int horizontalLines = 8;
    for (int i = 0; i <= horizontalLines; ++i) {
        const double ratio = static_cast<double>(i) / horizontalLines;
        const int y = plotRect.top() + static_cast<int>(ratio * plotRect.height());
        painter.drawLine(plotRect.left(), y, plotRect.right(), y);
    }
}

// 绘制坐标轴：X轴在底部，Y轴在左侧，以及Y轴刻度数值标签
void PlotWidget::drawAxes(QPainter& painter)
{
    const QRect plotRect = rect().adjusted(48, 20, -20, -36);

    painter.setPen(QPen(QColor(130, 136, 142), 1));  // 轴线颜色
    // X轴（底部水平线）
    painter.drawLine(plotRect.left(), plotRect.bottom(), plotRect.right(), plotRect.bottom());
    // Y轴（左侧垂直线）
    painter.drawLine(plotRect.left(), plotRect.top(), plotRect.left(), plotRect.bottom());

    const double minY = resolveMinY();  // 获取Y轴最小值（固定或自动）
    const double maxY = resolveMaxY();  // 获取Y轴最大值（固定或自动）

    // 在左侧绘制Y轴刻度数值
    painter.setPen(QColor(180, 184, 188));
    // drawText 的 y 是文本基线，+12 可让顶部标签与边框保持间距。
    painter.drawText(6, plotRect.top() + 12, QString::number(maxY, 'f', 2));  // 顶部标签
    painter.drawText(6, plotRect.bottom(), QString::number(minY, 'f', 2));    // 底部标签
}

// 绘制波形曲线：遍历所有通道，绘制QPainterPath
void PlotWidget::drawCurves(QPainter& painter)
{
    if (!m_stream || m_stream->channelCount() == 0) {
        return;  // 无数据直接返回
    }

    // 预定义4种通道颜色（绿、黄、蓝、红）
    const QVector<QColor> colors = {
        QColor(80, 220, 120),   // CH1: 绿色
        QColor(255, 214, 90),   // CH2: 黄色
        QColor(80, 170, 255),  // CH3: 蓝色
        QColor(255, 120, 150)   // CH4: 红色
    };

    // 遍历每个通道绘制曲线
    for (int channelIndex = 0; channelIndex < m_stream->channelCount(); ++channelIndex) {
        const StreamChannel* channel = m_stream->channel(channelIndex);
        if (!channel || !channel->isVisible()) {
            continue;  // 跳过不可见通道
        }

        QVector<double> values = channel->values();
        if (values.size() < 2) {
            continue;  // 数据点少于2个无法绘制曲线
        }

        // 如果数据点超过采样窗口，截取最新的采样窗口数据
        if (values.size() > m_sampleWindow) {
            values = values.mid(values.size() - m_sampleWindow);
        }

        // 构建QPainterPath，从第一个点移动到起点
        QPainterPath path;
        path.moveTo(mapSampleToPoint(0, values.size(), values[0]));

        // 逐点连线
        for (int i = 1; i < values.size(); ++i) {
            path.lineTo(mapSampleToPoint(i, values.size(), values[i]));
        }

        // 设置通道颜色（循环使用）
        painter.setPen(QPen(colors[channelIndex % colors.size()], 2));
        painter.drawPath(path);  // 绘制完整路径
    }
}

// 绘制标签：通道名称、采样窗口信息、暂停状态、无数据提示
void PlotWidget::drawLabels(QPainter& painter)
{
    const QRect plotRect = rect().adjusted(48, 20, -20, -36);

    // 左下角显示采样窗口数
    painter.setPen(QColor(210, 214, 218));
    painter.drawText(plotRect.left(), height() - 12, QString("Samples: %1").arg(m_sampleWindow));

    // 暂停时在右下角显示"Paused"标签
    if (m_paused) {
        painter.setPen(QColor(255, 214, 90));  // 黄色
        painter.drawText(plotRect.right() - 60, height() - 12, "Paused");
    }

    // 无数据流时显示提示
    if (!m_stream) {
        painter.setPen(QColor(180, 184, 188));
        painter.drawText(plotRect.center(), "No stream");
        return;
    }

    // 左上角绘制各通道名称标签
    int labelX = plotRect.left();
    const int labelY = plotRect.top() - 6;  // 在绘图区域上方

    for (int i = 0; i < m_stream->channelCount(); ++i) {
        const StreamChannel* channel = m_stream->channel(i);
        if (!channel) {
            continue;
        }

        // 使用通道名称，默认为CH1、CH2...
        const QString name = channel->name().isEmpty()
            ? QString("CH%1").arg(i + 1)
            : channel->name();

        // 使用通道颜色，无效则用默认灰色
        painter.setPen(channel->color().isValid() ? channel->color() : QColor(210, 214, 218));
        painter.drawText(labelX, labelY, name);
        labelX += 48;  // 每个标签间隔48像素
    }
}

// 解析Y轴最小值：自动缩放时从所有通道数据中找最小值，否则用固定值
double PlotWidget::resolveMinY() const
{
    if (!m_autoScaleY || !m_stream) {
        return m_minY;  // 固定模式
    }

    bool hasValue = false;
    double minValue = 0.0;

    // 遍历所有通道和所有数据点，找最小值
    for (int channelIndex = 0; channelIndex < m_stream->channelCount(); ++channelIndex) {
        const StreamChannel* channel = m_stream->channel(channelIndex);
        if (!channel || !channel->isVisible()) {
            continue;
        }

        const QVector<double> values = channel->values();
        for (double value : values) {
            if (!hasValue || value < minValue) {
                minValue = value;
                hasValue = true;
            }
        }
    }

    // 没有可见通道数据时，回退到手动设置的 Y 轴范围。
    return hasValue ? minValue : m_minY;
}

// 解析Y轴最大值：自动缩放时从所有通道数据中找最大值，否则用固定值
double PlotWidget::resolveMaxY() const
{
    if (!m_autoScaleY || !m_stream) {
        return m_maxY;  // 固定模式
    }

    bool hasValue = false;
    double maxValue = 0.0;

    // 遍历所有通道和所有数据点，找最大值
    for (int channelIndex = 0; channelIndex < m_stream->channelCount(); ++channelIndex) {
        const StreamChannel* channel = m_stream->channel(channelIndex);
        if (!channel || !channel->isVisible()) {
            continue;
        }

        const QVector<double> values = channel->values();
        for (double value : values) {
            if (!hasValue || value > maxValue) {
                maxValue = value;
                hasValue = true;
            }
        }
    }

    // 没有可见通道数据时，回退到手动设置的 Y 轴范围。
    return hasValue ? maxValue : m_maxY;
}

// 将数据点映射到屏幕坐标
// sampleIndex: 数据点在序列中的索引（0 ~ sampleCount-1）
// sampleCount: 数据点总数
// value: 数据值（范围[minY, maxY]）
// 返回值：对应的屏幕坐标QPointF
QPointF PlotWidget::mapSampleToPoint(int sampleIndex, int sampleCount, double value) const
{
    const QRect plotRect = rect().adjusted(48, 20, -20, -36);

    // 数据只有一个点时，居中显示
    if (sampleCount <= 1) {
        return QPointF(plotRect.left(), plotRect.center().y());
    }

    double minY = resolveMinY();
    double maxY = resolveMaxY();

    // 避免 Y 轴跨度为 0，防止映射时出现除零。
    if (qFuzzyCompare(minY, maxY)) {
        minY -= 1.0;
        maxY += 1.0;
    }

    // X方向：索引i映射到plotRect的宽度（从左到右）
    const double xRatio = static_cast<double>(sampleIndex) / static_cast<double>(sampleCount - 1);
    // Y方向：数值value映射到plotRect的高度（从上到下，注意Y轴翻转）
    const double yRatio = (value - minY) / (maxY - minY);

    const double x = plotRect.left() + xRatio * plotRect.width();
    const double y = plotRect.bottom() - yRatio * plotRect.height();  // Y轴翻转：较大值在上

    return QPointF(x, y);
}
