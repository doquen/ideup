#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFileInfo fi(a.applicationDirPath() + "/../share/icons/hicolor/256x256/apps/IDEup.png");
    QGuiApplication::setWindowIcon(QIcon(fi.absoluteFilePath()));
    MainWindow w;
    w.showMaximized();
    return a.exec();    
}
