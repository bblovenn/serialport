#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "core/stream.h"
#include "plot/plotwidget.h"
#include "readers/asciireader.h"
#include "serial/serialcontroller.h"

#include <QCheckBox>
#include <QDateTime>
#include <QFont>
#include <QIODevice>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollBar>
#include <QSizePolicy>
#include <QSpinBox>
#include <QStatusBar>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>

// 主窗口在构造时完成四件事：
// 1. 创建绘图组件
// 2. 创建串口与读取器
// 3. 建立界面和数据流连接
// 4. 初始化默认 UI 状态
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
    setupSerialConsoleConnections();
    applyVisualStyle();
    initializeUiState();
    refreshPorts();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupPlot()
{
    // Stream 保存多通道采样数据，PlotWidget 负责把它画出来。
    m_stream = new Stream(this);
    m_stream->setSampleWindow(500);

    QWidget* plotContainer = new QWidget(ui->widget_plotArea);
    plotContainer->setObjectName(QStringLiteral("plotContainer"));
    plotContainer->setMinimumSize(640, 320);
    plotContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->widget_plotArea->setMinimumHeight(360);

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
    plotAreaLayout->setContentsMargins(8, 8, 8, 8);
    plotAreaLayout->setSpacing(0);
    plotAreaLayout->addWidget(plotContainer, 1);
    ui->widget_plotArea->setLayout(plotAreaLayout);
}

void MainWindow::applyVisualStyle()
{
    QFont uiFont(QStringLiteral("Microsoft YaHei"));
    uiFont.setStyleHint(QFont::SansSerif);
    uiFont.setPointSize(10);
    uiFont.setWeight(QFont::Light);
    setFont(uiFont);
    ui->centralwidget->setFont(uiFont);

    setStyleSheet(QStringLiteral(R"(
        QMainWindow {
            background: #eef2f5;
        }

        QWidget#centralwidget {
            background: #eef2f5;
            color: #334155;
        }

        QPushButton {
            min-height: 34px;
            padding: 5px 18px;
            border: 1px solid #cbd5e1;
            border-radius: 8px;
            background: qlineargradient(
                x1:0, y1:0, x2:0, y2:1,
                stop:0 #ffffff,
                stop:1 #f4f7fb
            );
            color: #334155;
            font-weight: 400;
        }

        QPushButton:hover {
            background: qlineargradient(
                x1:0, y1:0, x2:0, y2:1,
                stop:0 #ffffff,
                stop:1 #eef4fb
            );
            border-color: #94a3b8;
            color: #1f2937;
        }

        QPushButton:pressed {
            background: #e2e8f0;
            border-color: #94a3b8;
            padding-top: 6px;
            padding-bottom: 4px;
        }

        QPushButton:disabled {
            background: #e2e8ee;
            color: #8b97a3;
            border-color: #d5dde5;
        }

        QPushButton#pushButton_openClose {
            min-width: 104px;
            background: qlineargradient(
                x1:0, y1:0, x2:0, y2:1,
                stop:0 #ffffff,
                stop:1 #eff6ff
            );
            border: 1px solid #93c5fd;
            border-left: 4px solid #3b82f6;
            border-radius: 8px;
            color: #1d4ed8;
            font-weight: 500;
        }

        QPushButton#pushButton_openClose:hover {
            background: qlineargradient(
                x1:0, y1:0, x2:0, y2:1,
                stop:0 #f8fbff,
                stop:1 #dbeafe
            );
            border: 1px solid #60a5fa;
            border-left: 4px solid #2563eb;
            color: #1e40af;
        }

        QPushButton#pushButton_openClose:pressed {
            background: #dbeafe;
            border: 1px solid #3b82f6;
            border-left: 4px solid #1d4ed8;
            color: #1e3a8a;
        }

        QPushButton#pushButton_openClose:disabled {
            background: #edf2f7;
            border: 1px solid #cbd5e1;
            border-left: 4px solid #cbd5e1;
            color: #94a3b8;
        }

        QPushButton#pushButton_openClose[mode="danger"] {
            background: qlineargradient(
                x1:0, y1:0, x2:0, y2:1,
                stop:0 #ffffff,
                stop:1 #fff7ed
            );
            border: 1px solid #fed7aa;
            border-left: 4px solid #f97316;
            color: #c2410c;
        }

        QPushButton#pushButton_openClose[mode="danger"]:hover {
            background: #ffedd5;
            border: 1px solid #fdba74;
            border-left: 4px solid #ea580c;
            color: #9a3412;
        }

        QPushButton#pushButton_openClose[mode="danger"]:pressed {
            background: #fed7aa;
            border: 1px solid #fb923c;
            border-left: 4px solid #c2410c;
            color: #7c2d12;
        }

        QPushButton#pushButton_clear {
            background: #fff7f7;
            border-color: #fecdd3;
            color: #be123c;
        }

        QPushButton#pushButton_clear:hover {
            background: #fff1f2;
            border-color: #fda4af;
            color: #9f1239;
        }

        QPushButton#pushButton_clear:pressed {
            background: #ffe4e6;
            border-color: #fb7185;
        }

        QComboBox, QSpinBox, QLineEdit {
            min-height: 30px;
            padding: 4px 8px;
            border: 1px solid #c8d1dc;
            border-radius: 6px;
            background: #ffffff;
            selection-background-color: #bfdbfe;
            selection-color: #1e3a8a;
        }

        QComboBox:disabled, QSpinBox:disabled, QLineEdit:disabled {
            background: #e2e8ee;
            color: #8b97a3;
        }

        QCheckBox {
            spacing: 6px;
            color: #263645;
        }

        QLabel {
            color: #475569;
        }

        QLabel#label_runtimeState {
            min-width: 92px;
            padding: 5px 12px;
            border-radius: 14px;
            font-weight: 700;
        }

        QWidget#widget_plotArea {
            background: #eef2f5;
        }

        QWidget#plotContainer {
            background: #101418;
            border: 1px solid #c7d0db;
            border-radius: 14px;
        }

        QGroupBox#groupBox_serialConsole {
            margin-top: 14px;
            padding-top: 12px;
            border: 1px solid #c8d1dc;
            border-radius: 10px;
            background: #f8fafc;
            font-weight: 500;
            color: #475569;
        }

        QGroupBox#groupBox_serialConsole::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 8px;
            background: #f8fafc;
        }

        QPlainTextEdit#plainTextEdit_serialLog {
            border: 1px solid #d0d8e2;
            border-radius: 8px;
            background: #ffffff;
            color: #334155;
            padding: 8px;
            selection-background-color: #d9f99d;
        }

        QStatusBar {
            background: #e5ebf1;
            color: #455565;
            border-top: 1px solid #c8d1dc;
        }
    )"));

    QFont logFont(QStringLiteral("Consolas"));
    logFont.setStyleHint(QFont::Monospace);
    logFont.setPointSize(10);
    logFont.setWeight(QFont::Normal);
    ui->plainTextEdit_serialLog->setFont(logFont);
}

void MainWindow::setupConnections()
{
    // 波形主链路：读取器解析出采样包后，直接追加到数据流。
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

void MainWindow::setupSerialConsoleConnections()
{
    // 通信面板相关操作统一放在这里，便于后续扩展发送和日志功能。
    connect(ui->pushButton_send, &QPushButton::clicked, this, &MainWindow::handleSendText);
    connect(ui->lineEdit_sendText, &QLineEdit::returnPressed, this, &MainWindow::handleSendText);
    connect(m_asciiReader, &AsciiReader::rawLineReceived, this, &MainWindow::handleRawLineReceived);
}

void MainWindow::setupSerial()
{
    // AsciiReader 只关心 QIODevice，因此把串口对象以设备形式交给它。
    m_asciiReader = new AsciiReader(this);
    m_serialController = new SerialController(this);
    m_asciiReader->setDevice(static_cast<QIODevice*>(m_serialController->port()));
}

void MainWindow::initializeUiState()
{
    // 这里集中设置启动时的默认界面状态，避免在构造函数里零散赋值。
    ui->pushButton_pause->setText(tr("暂停接收"));
    ui->pushButton_clear->setText(tr("清空波形"));
    ui->checkBox_autoScaleY->setChecked(false);
    ui->checkBox_autoScaleY->setText(tr("Y 轴自动缩放"));
    ui->label_port->setText(tr("串口："));
    ui->label_baudRate->setText(tr("波特率："));
    ui->label_sampleWindow->setText(tr("采样窗口："));
    ui->groupBox_serialConsole->setTitle(tr("串口通信面板"));
    ui->plainTextEdit_serialLog->setPlaceholderText(tr("串口收发日志将在这里显示"));
    ui->label_sendText->setText(tr("发送内容："));
    ui->lineEdit_sendText->setPlaceholderText(tr("请输入要发送的文本内容"));
    ui->checkBox_appendCrLf->setText(tr("自动附加回车换行"));
    ui->pushButton_send->setText(tr("发送数据"));
    ui->spinBox_sampleWindow->setValue(m_stream->sampleWindow());
    ui->pushButton_refreshPorts->setText(tr("刷新串口"));
    ui->pushButton_openClose->setText(tr("打开串口"));
    setOpenCloseButtonMode(false);
    ui->comboBox_baud->setCurrentText("115200");

    setRuntimeStateText(tr("未连接"));
    setSerialSettingsEnabled(true);

    showStatusMessage(tr("系统已就绪"));
}

void MainWindow::setRuntimeStateText(const QString& text)
{
    ui->label_runtimeState->setText(text);

    QString background = QStringLiteral("#e5e7eb");
    QString foreground = QStringLiteral("#475569");
    QString border = QStringLiteral("#cbd5e1");

    if (text.contains(tr("已连接"))) {
        background = QStringLiteral("#e7f8ef");
        foreground = QStringLiteral("#287a4b");
        border = QStringLiteral("#9bd8b5");
    } else if (text.contains(tr("已暂停"))) {
        background = QStringLiteral("#fff7df");
        foreground = QStringLiteral("#9a6a16");
        border = QStringLiteral("#efd27c");
    }

    ui->label_runtimeState->setStyleSheet(QStringLiteral(
        "min-width:92px;padding:5px 12px;border-radius:14px;"
        "font-weight:500;background:%1;color:%2;border:1px solid %3;"
    ).arg(background, foreground, border));
}

void MainWindow::setSerialSettingsEnabled(bool enabled)
{
    ui->comboBox_port->setEnabled(enabled);
    ui->comboBox_baud->setEnabled(enabled);
    ui->pushButton_refreshPorts->setEnabled(enabled);
}

void MainWindow::setOpenCloseButtonMode(bool connected)
{
    ui->pushButton_openClose->setProperty(
        "mode",
        connected ? QStringLiteral("danger") : QStringLiteral("primary")
    );
    ui->pushButton_openClose->style()->unpolish(ui->pushButton_openClose);
    ui->pushButton_openClose->style()->polish(ui->pushButton_openClose);
    ui->pushButton_openClose->update();
}

void MainWindow::refreshPorts()
{
    // 刷新串口列表时尽量保留用户当前选中的端口。
    const QString currentPort = ui->comboBox_port->currentText();
    const QStringList ports = m_serialController->availablePorts();

    ui->comboBox_port->clear();

    if (ports.isEmpty()) {
        ui->comboBox_port->addItem(tr("未发现串口"));
        ui->comboBox_port->setEnabled(false);
        ui->comboBox_baud->setEnabled(false);
        ui->pushButton_openClose->setEnabled(false);
        setRuntimeStateText(tr("未连接"));
        showStatusMessage(tr("未发现可用串口"), 3000);
        return;
    }

    ui->comboBox_port->setEnabled(true);
    ui->comboBox_baud->setEnabled(true);
    ui->pushButton_openClose->setEnabled(true);
    ui->comboBox_port->addItems(ports);

    if (!currentPort.isEmpty()) {
        const int index = ui->comboBox_port->findText(currentPort);
        if (index >= 0) {
            ui->comboBox_port->setCurrentIndex(index);
        }
    }

    showStatusMessage(tr("串口列表已更新"), 2000);
}

void MainWindow::startSerialMode()
{
    // 只有在未暂停时才真正启动读取器。
    setRuntimeStateText(tr("串口已连接"));

    if (!m_paused) {
        m_asciiReader->start();
    }
}

void MainWindow::stopSerialMode()
{
    m_asciiReader->stop();
}

void MainWindow::appendSerialLog(const QString& category, const QString& text)
{
    // 所有日志都统一加上类别和时间戳，方便排查通信过程。
    const QString timeText = QDateTime::currentDateTime().toString("HH:mm:ss");
    const QString line = QString("[%1 %2] %3").arg(category, timeText, text);

    ui->plainTextEdit_serialLog->appendPlainText(line);

    QScrollBar* scrollBar = ui->plainTextEdit_serialLog->verticalScrollBar();
    if (scrollBar) {
        scrollBar->setValue(scrollBar->maximum());
    }
}

void MainWindow::appendSystemLog(const QString& text)
{
    appendSerialLog(QStringLiteral("系统"), text);
}

void MainWindow::showStatusMessage(const QString& text, int timeoutMs)
{
    statusBar()->showMessage(text, timeoutMs);
}

void MainWindow::togglePause()
{
    // 暂停既影响波形显示，也影响底层文本读取。
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

        showStatusMessage(tr("已暂停"), 2000);
        return;
    }

    ui->pushButton_pause->setText(tr("暂停接收"));

    if (m_serialController->isOpen()) {
        m_asciiReader->start();
        setRuntimeStateText(tr("串口已连接"));
    } else {
        setRuntimeStateText(tr("未连接"));
    }

    showStatusMessage(tr("接收已恢复"), 2000);
}

void MainWindow::clearPlot()
{
    m_stream->clear();
    m_plotWidget->clear();
    showStatusMessage(tr("波形已清空"), 2000);
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
    // 打开和关闭共用一个按钮，按照当前串口状态切换行为。
    if (m_serialController->isOpen()) {
        m_serialController->close();
        m_stream->clear();
        return;
    }

    const QString portName = ui->comboBox_port->currentText().trimmed();
    const int baudRate = ui->comboBox_baud->currentText().toInt();

    if (portName.isEmpty()) {
        showStatusMessage(tr("请先选择串口"), 3000);
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
        setOpenCloseButtonMode(false);
        setSerialSettingsEnabled(true);
        appendSystemLog(tr("无法打开 %1：%2").arg(portName, errorText));
        showStatusMessage(tr("无法打开 %1：%2").arg(portName, errorText), 5000);
        return;
    }

    startSerialMode();
}

void MainWindow::handleSerialOpened(const QString& portName, int baudRate)
{
    // 串口连接成功后，锁定端口和波特率配置，避免运行中误切换。
    ui->pushButton_openClose->setText(tr("关闭串口"));
    setOpenCloseButtonMode(true);
    setRuntimeStateText(tr("串口已连接"));
    setSerialSettingsEnabled(false);

    const QString message = tr("已连接 %1，波特率 %2").arg(portName).arg(baudRate);
    appendSystemLog(message);
    showStatusMessage(message, 3000);
}

void MainWindow::handleSerialClosed()
{
    // 主动关闭和异常关闭最终都会回到这个状态收口函数。
    stopSerialMode();
    ui->pushButton_openClose->setText(tr("打开串口"));
    setOpenCloseButtonMode(false);
    setSerialSettingsEnabled(true);

    if (m_paused) {
        setRuntimeStateText(tr("已暂停"));
    } else {
        setRuntimeStateText(tr("未连接"));
    }

    appendSystemLog(tr("串口连接已关闭"));
    showStatusMessage(tr("串口连接已关闭"), 3000);
}

void MainWindow::handleSerialError(const QString& message)
{
    // 错误信息同时写到日志和状态栏，方便用户观察，也方便调试。
    appendSystemLog(tr("串口异常：%1").arg(message));
    showStatusMessage(tr("串口通信异常：%1").arg(message), 5000);

    if (!m_serialController->isOpen()) {
        stopSerialMode();
        ui->pushButton_openClose->setText(tr("打开串口"));
        setOpenCloseButtonMode(false);
        setSerialSettingsEnabled(true);

        if (m_paused) {
            setRuntimeStateText(tr("已暂停"));
        } else {
            setRuntimeStateText(tr("未连接"));
        }
    }
}

void MainWindow::handleSendText()
{
    // 发送流程：校验输入 -> 校验串口状态 -> 根据选项补回车换行 -> 写入串口。
    const QString inputText = ui->lineEdit_sendText->text();

    if (inputText.trimmed().isEmpty()) {
        showStatusMessage(tr("请输入发送内容"), 3000);
        return;
    }

    if (!m_serialController->isOpen()) {
        showStatusMessage(tr("请先打开串口"), 3000);
        return;
    }

    QString sendText = inputText;
    if (ui->checkBox_appendCrLf->isChecked()) {
        sendText.append("\r\n");
    }

    if (!m_serialController->send(sendText.toUtf8())) {
        const QString errorText = m_serialController->lastErrorString().isEmpty()
            ? tr("发送失败")
            : m_serialController->lastErrorString();

        appendSystemLog(tr("发送失败：%1").arg(errorText));
        showStatusMessage(tr("发送失败：%1").arg(errorText), 5000);
        return;
    }

    appendSerialLog(QStringLiteral("发送"), inputText);
    showStatusMessage(tr("数据已发送"), 2000);
}

void MainWindow::handleRawLineReceived(const QString& text)
{
    // 原始文本日志和数值解析分开处理，互不影响。
    appendSerialLog(QStringLiteral("接收"), text);
}
