#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "simplefiledialog.h"
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
    connect(this,SIGNAL(transferFile(QString,QByteArray)),term,SLOT(saveTargetFile(QString,QByteArray)));
    connect(targ,SIGNAL(createNewTargetFile(QString,QByteArray)),term,SLOT(saveTargetFile(QString,QByteArray)));
    connect(targ,SIGNAL(createNewTargetDir(QString)),term,SLOT(createTargetDir(QString)));
    connect(targ,SIGNAL(deleteTargetDir(QString)),term,SLOT(deleteTargetDir(QString)));
    connect(targ,SIGNAL(deleteTargetFile(QString)),term,SLOT(deleteTargetFile(QString)));
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
    if(et->getFilePath() != "")
        et->saveFile(QDir::cleanPath(et->getFilePath()));
    else
        on_actionGuardar_Como_triggered();
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

void MainWindow::on_actionTransferir_triggered()
{
    EditorTab *et = ed->getCurrentTab();
    QString fileName = et->getFilePath().split('/').last();

    if (et->is_host_file()){
        transferFile(fileName,et->ed->text().toLatin1());
    }else{
        QFile f(fileName);
        f.open(QIODevice::WriteOnly);
        f.write(et->ed->text().toLatin1());
        f.close();
    }
}

void MainWindow::on_actionGuardar_Como_triggered()
{
    QString fileName;
    EditorTab *et = ed->getCurrentTab();
    SimpleFileDialog sfd(&fileName,"Guardar Como",true);
    if(sfd.exec()){
        et->set_host_file(sfd.host_selected());
        if (sfd.host_selected()){
            et->saveFile(fileName);
            ed->getTabWidget()->setTabText(ed->getTabWidget()->currentIndex(),QDir().absoluteFilePath(fileName));
        }
        else{
            QString path = term->get_target_current_dir();
            path = path == "/" ? path + fileName : path + "/" + fileName;
            ed->getTabWidget()->setTabText(ed->getTabWidget()->currentIndex(),"(target)"+path);
            et->saveFile(path);
        }
    }
}

void MainWindow::on_actionArchivo_Nuevo_triggered()
{
    QString name;
    SimpleFileDialog sfd(&name,"Nuevo");
    if(sfd.exec()){
        ed->addTab(new EditorTab(),name);
    }
}
