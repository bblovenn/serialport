#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class PlotWidget;
class Stream;
class AsciiReader;
class SerialController;

namespace Ui {
class MainWindow;
}

// MainWindow 负责把串口控制、数据读取、波形显示和通信面板组织在一起。
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    void setupPlot();
    void setupConnections();
    void setupSerialConsoleConnections();
    void setupSerial();
    void initializeUiState();
    void applyVisualStyle();

    void setRuntimeStateText(const QString& text);
    void setSerialSettingsEnabled(bool enabled);
    void setOpenCloseButtonMode(bool connected);
    void refreshPorts();
    void startSerialMode();
    void stopSerialMode();

    void appendSerialLog(const QString& category, const QString& text);
    void appendSystemLog(const QString& text);
    void showStatusMessage(const QString& text, int timeoutMs = 3000);

private slots:
    void togglePause();
    void clearPlot();
    void changeAutoScaleY(bool enabled);
    void changeSampleWindow(int samples);
    void handleRefreshPorts();
    void toggleSerialPort();
    void handleSerialOpened(const QString& portName, int baudRate);
    void handleSerialClosed();
    void handleSerialError(const QString& message);
    void handleSendText();
    void handleRawLineReceived(const QString& text);

private:
    Ui::MainWindow* ui;

    PlotWidget* m_plotWidget;
    Stream* m_stream;
    AsciiReader* m_asciiReader;
    SerialController* m_serialController;

    bool m_paused; // 暂停状态同时影响串口读取和波形刷新文案。
};

#endif // MAINWINDOW_H
