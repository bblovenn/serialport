#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "core/stream.h"
#include "plot/plotwidget.h"
#include "readers/demoreader.h"

#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_demoReader(nullptr)
    , m_plotWidget(nullptr)
    , m_stream(nullptr)
{
    ui->setupUi(this);
    setWindowTitle(tr("SerialPlot Rebuild"));

    setupPlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupPlot()
{
    m_stream = new Stream(this);
    m_stream->setSampleWindow(500);

    m_demoReader = new DemoReader(this);

    m_plotWidget = new PlotWidget(this);
    m_plotWidget->setStream(m_stream);
    m_plotWidget->setSampleWindow(m_stream->sampleWindow());
    m_plotWidget->setYAxisRange(-1.2, 1.2);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(8, 8, 8, 8);
    layout->addWidget(m_plotWidget);

    ui->centralwidget->setLayout(layout);

    connect(
        m_demoReader,
        &DemoReader::samplesReady,
        m_stream,
        &Stream::appendSamples
    );

    m_demoReader->start();
}
