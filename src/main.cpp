#include "mainwindow.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char* argv[])
{
    // Qt Widgets 程序入口：先创建应用对象，再创建并显示主窗口。
    QApplication app(argc, argv);  
    app.setWindowIcon(QIcon(QStringLiteral(":/icons/app_icon.png")));

    MainWindow window;
    window.setWindowIcon(QIcon(QStringLiteral(":/icons/app_icon.png")));
    window.show();

    return app.exec();
}
