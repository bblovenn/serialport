#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(QStringLiteral(":/icons/app_icon.png")));

    MainWindow window;
    window.setWindowIcon(QIcon(QStringLiteral(":/icons/app_icon.png")));
    window.show();

    return app.exec();
}
