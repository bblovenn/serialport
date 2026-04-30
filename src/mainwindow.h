#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QByteArray>
#include <QMainWindow>
#include <QString>

#include "protocol/protocolframe.h"
#include "storage/csvrecorder.h"

class PlotWidget;
class Stream;
class AsciiReader;
class SerialController;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    enum class RuntimeState {
        Disconnected,
        Connected,
        Paused
    };

    void setupPlot();
    void setupConnections();
    void setupSerialConsoleConnections();
    void setupSerial();
    void initializeUiState();
    void applyVisualStyle();

    void setRuntimeStateText(const QString& text);
    void setRuntimeState(RuntimeState state, const QString& text);
    void setSerialSettingsEnabled(bool enabled);
    void setOpenCloseButtonMode(bool connected);
    void refreshPorts();
    void startSerialMode();
    void stopSerialMode();

    void appendSerialLog(const QString& category, const QString& text);
    void appendSystemLog(const QString& text);
    void showStatusMessage(const QString& text, int timeoutMs = 3000);
    void updateRecordUiState();

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
    void handleProtocolFrameParsed(const ProtocolFrame& frame);
    void handleProtocolParseError(const QString& message);
    void handleStartRecord();
    void handleStopRecord();
    void handleClearSerialLog();

private:
    Ui::MainWindow* ui;

    PlotWidget* m_plotWidget;
    Stream* m_stream;
    AsciiReader* m_asciiReader;
    SerialController* m_serialController;
    CsvRecorder m_csvRecorder;
    QString m_currentRecordPath;

    RuntimeState m_runtimeState;
    bool m_paused;
};

#endif // MAINWINDOW_H
