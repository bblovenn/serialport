#include "plotwidget.h"

// 构造函数，初始化绘图控件
PlotWidget::PlotWidget(QWidget *parent)
    : QWidget(parent)
    , m_stream(nullptr)
    , m_sampleWindow(1000)
    , m_autoScaleY(false)
    , m_minY(-1.2)
    , m_maxY(1.2)
    , m_paused(false)
{
    setMinimumSize(600, 300);      // 设置最小尺寸
    setAutoFillBackground(false);  // 允许透明背景

    // 连接定时器，每33ms刷新一次（约30fps）
    connect(&m_refreshTimer, &QTimer::timeout, this, QOverload<>::of(&PlotWidget::update));
    m_refreshTimer.start(33);
}

// 绑定数据流
void PlotWidget::setStream(Stream *stream)
{
    m_stream = stream;
    update();  // 立即刷新
}

// 设置采样窗口
void PlotWidget::setSampleWindow(int samples)
{
    m_sampleWindow = qMax(1, samples);
    update();
}

// 设置Y轴自动缩放
void PlotWidget::setAutoScaleY(bool enabled)
{
    m_autoScaleY = enabled;
    update();
}

// 设置Y轴范围
void PlotWidget::setYAxisRange(double min, double max)
{
    if (min == max) {
        return;  // 避免无效范围
    }

    m_minY = qMin(min, max);  // 确保 min < max
    m_maxY = qMax(min, max);
    update();
}

// 清空数据
void PlotWidget::clear()
{
    if (m_stream) {
        m_stream->clear();  // 清空流数据
    }
    update();
}

// 设置暂停状态
void PlotWidget::setPaused(bool paused)
{
    m_paused = paused;

    if (m_stream) {
        m_stream->setPaused(paused);  // 同步暂停流
    }
    update();
}

// 绘制事件，绘制所有内容
void PlotWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);  // 开启抗锯齿

    drawBackground(painter);  // 绘制背景
    drawGrid(painter);        // 绘制网格
    drawAxes(painter);        // 绘制坐标轴
    drawCurves(painter);      // 绘制曲线
    drawLabels(painter);      // 绘制标签
}

// 绘制背景（待实现）
void PlotWidget::drawBackground(QPainter &painter)
{
}

// 绘制网格（待实现）
void PlotWidget::drawGrid(QPainter &painter)
{
}

// 绘制坐标轴（待实现）
void PlotWidget::drawAxes(QPainter &painter)
{
}

// 绘制曲线（待实现）
void PlotWidget::drawCurves(QPainter &painter)
{
}

// 绘制标签（待实现）
void PlotWidget::drawLabels(QPainter &painter)
{
}

// 计算Y轴最小值（待实现）
double PlotWidget::resolveMinY() const
{
}

// 计算Y轴最大值（待实现）
double PlotWidget::resolveMaxY() const
{
}

// 将采样点映射到坐标点（待实现）
QPointF PlotWidget::mapSampleToPoint(int sampleIndex, int sampleCount, double value) const
{
}
