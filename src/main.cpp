#include "mainwindow.h"
#include "core/stream.h"
#include "readers/demoreader.h"


#include <QApplication>
#include <QDebug>
#include <QTimer>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    

    MainWindow window;
    window.show();

    return app.exec();
}
