#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "core/stream.h"
#include "plot/plotwidget.h"
#include "readers/demoreader.h"

#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_demoReader(nullptr)
    , m_plotWidget(nullptr)
    , m_stream(nullptr)
    , m_paused(false)
{
    ui->setupUi(this);
    setWindowTitle(tr("SerialPlot Rebuild"));

    // Build the plotting pipeline before wiring UI controls to it.
    setupPlot();
    setupConnections();

    ui->pushButton_pause->setText(tr("Pause"));
    ui->pushButton_clear->setText(tr("Clear"));
    ui->checkBox_autoScaleY->setChecked(false);
    ui->spinBox_sampleWindow->setValue(m_stream->sampleWindow());

    m_demoReader->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupPlot()
{
    // Stream owns the rolling channel data displayed by the plot.
    m_stream = new Stream(this);
    m_stream->setSampleWindow(500);

    // DemoReader acts as the phase-one data source until serial input is used.
    m_demoReader = new DemoReader(this);

    m_plotWidget = new PlotWidget(ui->widget_plotArea);
    m_plotWidget->setStream(m_stream);
    m_plotWidget->setSampleWindow(m_stream->sampleWindow());
    m_plotWidget->setYAxisRange(-1.2, 1.2);
    m_plotWidget->setAutoScaleY(false);

    QVBoxLayout* plotLayout = new QVBoxLayout;
    plotLayout->setContentsMargins(0, 0, 0, 0);
    plotLayout->setSpacing(0);
    plotLayout->addWidget(m_plotWidget);

    // Mount the custom plot widget into the placeholder created in Designer.
    ui->widget_plotArea->setLayout(plotLayout);
}

void MainWindow::setupConnections()
{
    connect(m_demoReader, &DemoReader::samplesReady, m_stream, &Stream::appendSamples);
    connect(ui->pushButton_pause, &QPushButton::clicked, this, &MainWindow::togglePause);
    connect(ui->pushButton_clear, &QPushButton::clicked, this, &MainWindow::clearPlot);
    connect(ui->checkBox_autoScaleY, &QCheckBox::toggled, this, &MainWindow::changeAutoScaleY);
    connect(
        ui->spinBox_sampleWindow,
        QOverload<int>::of(&QSpinBox::valueChanged),
        this,
        &MainWindow::changeSampleWindow
    );
}

void MainWindow::togglePause()
{
    // Keep the reader, stream, and plot widget in the same paused state.
    m_paused = !m_paused;

    m_stream->setPaused(m_paused);
    m_plotWidget->setPaused(m_paused);

    if (m_paused) {
        m_demoReader->stop();
        ui->pushButton_pause->setText(tr("Resume"));
    } else {
        m_demoReader->start();
        ui->pushButton_pause->setText(tr("Pause"));
    }
}

void MainWindow::clearPlot()
{
    m_stream->clear();
    m_plotWidget->clear();
}

void MainWindow::changeAutoScaleY(bool enabled)
{
    m_plotWidget->setAutoScaleY(enabled);
}

void MainWindow::changeSampleWindow(int samples)
{
    m_stream->setSampleWindow(samples);
    m_plotWidget->setSampleWindow(samples);
}
