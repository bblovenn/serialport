#include "mainwindow.h"
#include "core/ringbuffer.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);


    
    MainWindow window;
    window.show();

    return app.exec();
}
