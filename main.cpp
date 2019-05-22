#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGuiApplication::setWindowIcon(QIcon(":/IDEup.png"));
    MainWindow w;
    w.showMaximized();
    return a.exec();    
}
