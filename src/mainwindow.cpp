#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "core/stream.h"
#include "plot/plotwidget.h"
#include "readers/asciireader.h"
#include "serial/serialcontroller.h"

#include <QCheckBox>
#include <QIODevice>
#include <QPushButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_plotWidget(nullptr)
    , m_stream(nullptr)
    , m_asciiReader(nullptr)
    , m_serialController(nullptr)
    , m_paused(false)
{
    ui->setupUi(this);
    setWindowTitle(tr("串口波形监视器"));

    setupPlot();
    setupSerial();
    setupConnections();
    initializeUiState();
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
    plotContainer->setMinimumSize(860, 440);
    plotContainer->setMaximumSize(1020, 560);

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
    plotAreaLayout->setContentsMargins(0, 4, 0, 4);
    plotAreaLayout->setSpacing(8);
    plotAreaLayout->addWidget(plotContainer, 0, Qt::AlignHCenter | Qt::AlignTop);
    plotAreaLayout->addStretch();
    ui->widget_plotArea->setLayout(plotAreaLayout);
}

void MainWindow::setupConnections()
{
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

    connect(
        m_serialController,
        &SerialController::serialOpened,
        this,
        &MainWindow::handleSerialOpened
    );
    connect(
        m_serialController,
        &SerialController::serialClosed,
        this,
        &MainWindow::handleSerialClosed
    );
    connect(
        m_serialController,
        &SerialController::serialErrorOccurred,
        this,
        &MainWindow::handleSerialError
    );
}

void MainWindow::setupSerial()
{
    m_asciiReader = new AsciiReader(this);
    m_serialController = new SerialController(this);
    m_asciiReader->setDevice(static_cast<QIODevice*>(m_serialController->port()));
}

void MainWindow::initializeUiState()
{
    ui->pushButton_pause->setText(tr("暂停接收"));
    ui->pushButton_clear->setText(tr("清空波形"));
    ui->checkBox_autoScaleY->setChecked(false);
    ui->spinBox_sampleWindow->setValue(m_stream->sampleWindow());
    ui->pushButton_refreshPorts->setText(tr("刷新串口"));
    ui->pushButton_openClose->setText(tr("打开串口"));
    ui->comboBox_baud->setCurrentText("115200");

    setRuntimeStateText(tr("未连接"));
    setSerialSettingsEnabled(true);

    statusBar()->showMessage(tr("系统已就绪"));
}

void MainWindow::setRuntimeStateText(const QString& text)
{
    ui->label_runtimeState->setText(text);
}

void MainWindow::setSerialSettingsEnabled(bool enabled)
{
    ui->comboBox_port->setEnabled(enabled);
    ui->comboBox_baud->setEnabled(enabled);
    ui->pushButton_refreshPorts->setEnabled(enabled);
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

    statusBar()->showMessage(tr("串口列表已更新"), 2000);
}

void MainWindow::startSerialMode()
{
    setRuntimeStateText(tr("串口已连接"));

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
        m_asciiReader->stop();
        ui->pushButton_pause->setText(tr("继续接收"));

        if (m_serialController->isOpen()) {
            setRuntimeStateText(tr("已暂停"));
        } else {
            setRuntimeStateText(tr("未连接"));
        }

        statusBar()->showMessage(tr("已暂停"), 2000);
        return;
    }

    ui->pushButton_pause->setText(tr("暂停接收"));

    if (m_serialController->isOpen()) {
        m_asciiReader->start();
        setRuntimeStateText(tr("串口已连接"));
    } else {
        setRuntimeStateText(tr("未连接"));
    }

    statusBar()->showMessage(tr("接收已恢复"), 2000);
}

void MainWindow::clearPlot()
{
    m_stream->clear();
    m_plotWidget->clear();
    statusBar()->showMessage(tr("波形已清空"), 2000);
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
        m_serialController->close();
        m_stream->clear();
        return;
    }

    const QString portName = ui->comboBox_port->currentText().trimmed();
    const int baudRate = ui->comboBox_baud->currentText().toInt();

    if (portName.isEmpty()) {
        statusBar()->showMessage(tr("请先选择串口"), 3000);
        return;
    }

    stopSerialMode();
    m_stream->clear();
    m_asciiReader->setDevice(static_cast<QIODevice*>(m_serialController->port()));

    if (!m_serialController->open(portName, baudRate)) {
        const QString errorText = m_serialController->lastErrorString().isEmpty()
            ? tr("未知错误")
            : m_serialController->lastErrorString();

        setRuntimeStateText(tr("未连接"));
        ui->pushButton_openClose->setText(tr("打开串口"));
        setSerialSettingsEnabled(true);
        statusBar()->showMessage(
            tr("无法打开 %1：%2").arg(portName).arg(errorText),
            5000
        );
        return;
    }

    startSerialMode();
}

void MainWindow::handleSerialOpened(const QString& portName, int baudRate)
{
    ui->pushButton_openClose->setText(tr("关闭串口"));
    setRuntimeStateText(tr("串口已连接"));
    setSerialSettingsEnabled(false);
    statusBar()->showMessage(tr("已连接 %1，波特率 %2").arg(portName).arg(baudRate), 3000);
}

void MainWindow::handleSerialClosed()
{
    stopSerialMode();
    ui->pushButton_openClose->setText(tr("打开串口"));
    setSerialSettingsEnabled(true);

    if (m_paused) {
        setRuntimeStateText(tr("已暂停"));
    } else {
        setRuntimeStateText(tr("未连接"));
    }

    statusBar()->showMessage(tr("串口连接已关闭"), 3000);
}

void MainWindow::handleSerialError(const QString& message)
{
    statusBar()->showMessage(tr("串口通信异常：%1").arg(message), 5000);

    if (!m_serialController->isOpen()) {
        stopSerialMode();
        ui->pushButton_openClose->setText(tr("打开串口"));
        setSerialSettingsEnabled(true);

        if (m_paused) {
            setRuntimeStateText(tr("已暂停"));
        } else {
            setRuntimeStateText(tr("未连接"));
        }
    }
}
