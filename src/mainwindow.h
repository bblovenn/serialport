#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

//前向声明
class DemoReader;
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
    void setupPlot();
    void setupConnections();
    void setupSerial();
    void initializeUiState();
    void setRuntimeStateText(const QString& text);
    void refreshPorts();
    void startDemoMode();
    void stopDemoMode();
    void startSerialMode();
    void stopSerialMode();

private slots:
    void togglePause();
    void clearPlot();
    void changeAutoScaleY(bool enabled);
    void changeSampleWindow(int samples);
    void handleRefreshPorts();
    void toggleSerialPort();

private:
    Ui::MainWindow* ui;

    DemoReader* m_demoReader;
    PlotWidget* m_plotWidget;
    Stream* m_stream;
    AsciiReader* m_asciiReader;
    SerialController* m_serialController;

    bool m_paused;
};

#endif // MAINWINDOW_H
