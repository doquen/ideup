#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ed = new Editor();
    term = new Terminal();
    host = new HostFileSystem();
    targ = new TargetFileSystem();
    this->setCentralWidget(ed);

    setStyleSheet("QDockWidget{color: rgb(235, 235, 235);border-color:black; border-width: 2px;}"
                  "QDockWidget::title{ background: rgb(40%, 40%, 50%); border-radius:4px;}");
    ui->dockWidget_5->setFeatures(QDockWidget::DockWidgetMovable);
    ui->dockWidget_5->setWindowTitle("Terminal");
    ui->dockWidget_5->setWidget(term);

    ui->dockWidget->setFeatures(QDockWidget::DockWidgetMovable);
    ui->dockWidget->setWindowTitle("Archivos PC");
    ui->dockWidget->setWidget(host);

    ui->dockWidget_2->setFeatures(QDockWidget::DockWidgetMovable);
    ui->dockWidget_2->setWindowTitle("Archivos Placa");
    ui->dockWidget_2->setWidget(targ);


    ed->addTab(new EditorTab(),"Nuevo");

    connect(term,SIGNAL(connected(QString,bool)),this,SLOT(port_connected(QString,bool)));
    connect(host,SIGNAL(openFile(QString)),ed,SLOT(openFile(QString)));
    connect(term,SIGNAL(sendScannedTargetFiles(QStringList,QStringList)),targ,SLOT(addFiles(QStringList,QStringList)));
    connect(targ,SIGNAL(currentDirChanged(QString)),term,SLOT(targetDirChanged(QString)));
    connect(term,SIGNAL(update_target_dir(QString)),targ,SLOT(on_current_dir_changed(QString)));
    connect(term,SIGNAL(port_connected(bool)),targ,SLOT(setEnabled(bool)));
    connect(targ,SIGNAL(openFile(QString)),term,SLOT(openTargetFile(QString)));
    connect(term,SIGNAL(targetFileOpened(QString,QString)),ed,SLOT(openTargetFile(QString,QString)));
    connect(ed,SIGNAL(saveTargetFile(QString,QByteArray)),term,SLOT(saveTargetFile(QString,QByteArray)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionEjecutar_Resaltado_triggered()
{
    EditorTab *et = ed->getCurrentTab();
    QByteArray data;
    data.append(0x05);
    data.append(et->ed->selectedText().toLatin1());
    data.append(0x04);
    term->writeData(data);
}

void MainWindow::port_connected(QString portName, bool state){
    if(state){
        ui->statusBar->setStyleSheet("color: blue");
        ui->statusBar->showMessage("Conexión Exitosa con el puerto "+portName,2000);
    }
    else{
        ui->statusBar->setStyleSheet("color: red");
        ui->statusBar->showMessage("Falló conexión con el puerto "+portName,2000);
    }
}

void MainWindow::on_actionGuardar_triggered()
{
    EditorTab *et= ed->getCurrentTab();
    et->saveFile(et->getFilePath());
}

void MainWindow::on_actionEjecutar_triggered()
{
    EditorTab *et = ed->getCurrentTab();
    QByteArray data;
    data.append(0x05);
    data.append(et->ed->text().toLatin1());
    data.append(0x04);
    term->writeData(data);
}
