#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "editor.h"
#include "editortab.h"
#include "terminal.h"
#include "hostfilesystem.h"
#include "targetfilesystem.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionEjecutar_Resaltado_triggered();
    void port_connected(QString portName, bool state);

    void on_actionGuardar_triggered();

    void on_actionEjecutar_triggered();

    void on_actionTransferir_triggered();
    void on_actionGuardar_Como_triggered();

signals:
    void transferFile(QString filename, QByteArray content);
private:
    Ui::MainWindow *ui;
    Editor *ed;
    Terminal *term;
    HostFileSystem *host;
    TargetFileSystem *targ;
};

#endif // MAINWINDOW_H
