#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "core/stream.h"
#include "plot/plotwidget.h"
#include "readers/asciireader.h"
#include "readers/demoreader.h"
#include "serial/serialcontroller.h"

#include <QCheckBox>
#include <QIODevice>
#include <QPushButton>
#include <QSerialPort>
#include <QSpinBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_demoReader(nullptr)
    , m_plotWidget(nullptr)
    , m_stream(nullptr)
    , m_asciiReader(nullptr)
    , m_serialController(nullptr)
    , m_paused(false)
{
    ui->setupUi(this);
    setWindowTitle(tr("SerialPlot Rebuild"));

    setupPlot();
    setupSerial();
    setupConnections();
    initializeUiState();

    startDemoMode();
    refreshPorts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupPlot()
{
    m_stream = new Stream(this);
    m_stream->setSampleWindow(500);

    QWidget* plotContainer = new QWidget(ui->widget_plotArea);
    plotContainer->setMaximumSize(1200, 700);
    plotContainer->setMinimumSize(600, 300);

    m_plotWidget = new PlotWidget(plotContainer);
    m_plotWidget->setStream(m_stream);
    m_plotWidget->setSampleWindow(m_stream->sampleWindow());
    m_plotWidget->setYAxisRange(-1.2, 1.2);
    m_plotWidget->setAutoScaleY(false);

    QVBoxLayout* containerLayout = new QVBoxLayout;
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(0);
    containerLayout->addWidget(m_plotWidget);
    plotContainer->setLayout(containerLayout);

    QVBoxLayout* plotAreaLayout = new QVBoxLayout;
    plotAreaLayout->setContentsMargins(0, 0, 0, 0);
    plotAreaLayout->setSpacing(0);
    plotAreaLayout->addWidget(plotContainer, 0, Qt::AlignCenter);
    ui->widget_plotArea->setLayout(plotAreaLayout);
}

void MainWindow::setupConnections()
{
    connect(m_demoReader, &DemoReader::samplesReady, m_stream, &Stream::appendSamples);
    connect(m_asciiReader, &AsciiReader::samplesReady, m_stream, &Stream::appendSamples);

    connect(ui->pushButton_pause, &QPushButton::clicked, this, &MainWindow::togglePause);
    connect(ui->pushButton_clear, &QPushButton::clicked, this, &MainWindow::clearPlot);
    connect(ui->checkBox_autoScaleY, &QCheckBox::toggled, this, &MainWindow::changeAutoScaleY);
    connect(
        ui->spinBox_sampleWindow,
        QOverload<int>::of(&QSpinBox::valueChanged),
        this,
        &MainWindow::changeSampleWindow
    );
    connect(
        ui->pushButton_refreshPorts,
        &QPushButton::clicked,
        this,
        &MainWindow::handleRefreshPorts
    );
    connect(
        ui->pushButton_openClose,
        &QPushButton::clicked,
        this,
        &MainWindow::toggleSerialPort
    );
}

void MainWindow::setupSerial()
{
    m_demoReader = new DemoReader(this);
    m_asciiReader = new AsciiReader(this);
    m_serialController = new SerialController(this);

    m_asciiReader->setDevice(static_cast<QIODevice*>(m_serialController->port()));
}

void MainWindow::initializeUiState()
{
    ui->pushButton_pause->setText(tr("Pause"));
    ui->pushButton_clear->setText(tr("Clear"));
    ui->checkBox_autoScaleY->setChecked(false);
    ui->spinBox_sampleWindow->setValue(m_stream->sampleWindow());

    ui->pushButton_refreshPorts->setText(tr("Refresh"));
    ui->pushButton_openClose->setText(tr("Open"));
    setRuntimeStateText(tr("Demo Mode"));

    ui->comboBox_baud->setCurrentText("115200");

    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::setRuntimeStateText(const QString& text)
{
    ui->label_runtimeState->setText(text);
}

void MainWindow::refreshPorts()
{
    const QString currentPort = ui->comboBox_port->currentText();
    const QStringList ports = m_serialController->availablePorts();

    ui->comboBox_port->clear();
    ui->comboBox_port->addItems(ports);

    if (!currentPort.isEmpty()) {
        const int index = ui->comboBox_port->findText(currentPort);
        if (index >= 0) {
            ui->comboBox_port->setCurrentIndex(index);
        }
    }

    statusBar()->showMessage(tr("Ports refreshed"), 2000);
}

void MainWindow::startDemoMode()
{
    setRuntimeStateText(tr("Demo Mode"));

    if (!m_paused) {
        m_demoReader->start();
    }
}

void MainWindow::stopDemoMode()
{
    m_demoReader->stop();
}

void MainWindow::startSerialMode()
{
    setRuntimeStateText(tr("Serial Connected"));

    if (!m_paused) {
        m_asciiReader->start();
    }
}

void MainWindow::stopSerialMode()
{
    m_asciiReader->stop();
}

void MainWindow::togglePause()
{
    m_paused = !m_paused;

    m_stream->setPaused(m_paused);
    m_plotWidget->setPaused(m_paused);

    if (m_paused) {
        m_demoReader->stop();
        m_asciiReader->stop();
        ui->pushButton_pause->setText(tr("Resume"));
        setRuntimeStateText(tr("Paused"));
        statusBar()->showMessage(tr("Paused"), 2000);
    } else {
        if (m_serialController->isOpen()) {
            m_asciiReader->start();
            setRuntimeStateText(tr("Serial Connected"));
        } else {
            m_demoReader->start();
            setRuntimeStateText(tr("Demo Mode"));
        }

        ui->pushButton_pause->setText(tr("Pause"));
        statusBar()->showMessage(tr("Running"), 2000);
    }
}

void MainWindow::clearPlot()
{
    m_stream->clear();
    m_plotWidget->clear();
    statusBar()->showMessage(tr("Plot cleared"), 2000);
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

void MainWindow::handleRefreshPorts()
{
    refreshPorts();
}

void MainWindow::toggleSerialPort()
{
    if (m_serialController->isOpen()) {
        stopSerialMode();
        m_serialController->close();

        ui->pushButton_openClose->setText(tr("Open"));
        statusBar()->showMessage(tr("Serial port closed"), 2000);

        m_stream->clear();
        startDemoMode();
        return;
    }

    const QString portName = ui->comboBox_port->currentText().trimmed();
    const int baudRate = ui->comboBox_baud->currentText().toInt();

    if (portName.isEmpty()) {
        statusBar()->showMessage(tr("Please select a serial port"), 3000);
        return;
    }

    stopDemoMode();
    stopSerialMode();
    m_stream->clear();

    if (!m_serialController->open(portName, baudRate)) {
        startDemoMode();
        setRuntimeStateText(tr("Demo Mode"));
        statusBar()->showMessage(tr("Failed to open %1").arg(portName), 3000);
        return;
    }

    m_asciiReader->setDevice(static_cast<QIODevice*>(m_serialController->port()));
    startSerialMode();
    setRuntimeStateText(tr("Serial Connected"));

    ui->pushButton_openClose->setText(tr("Close"));
    statusBar()->showMessage(tr("Opened %1 @ %2").arg(portName).arg(baudRate), 3000);
}
