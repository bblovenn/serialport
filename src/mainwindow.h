#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//前向声明
class DemoReader;
class PlotWidget;
class Stream;

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

private:
    Ui::MainWindow* ui;
    DemoReader* m_demoReader;
    PlotWidget* m_plotWidget;
    Stream* m_stream;
};

#endif // MAINWINDOW_H
