#include "hostfilesystem.h"
#include "ui_hostfilesystem.h"
#include "simplefiledialog.h"
#include <QFileSystemModel>
#include <QKeyEvent>
#include <QDebug>

HostFileSystem::HostFileSystem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HostFileSystem)
{
    ui->setupUi(this);
    model = new QFileSystemModel;
    model->setRootPath(QDir::currentPath());

    ui->treeView->setModel(model);
    ui->treeView->setRootIndex(model->index(QDir::currentPath()));

    QStringList dirs = QDir::current().entryList(QDir::Dirs);
    ui->comboBox->addItems(dirs);
    ui->label->setText(QDir::cleanPath(model->rootPath()));

}

HostFileSystem::~HostFileSystem()
{
    delete ui;
}

void HostFileSystem::fillDirBox(){
    QDir dir = QDir(model->rootPath());
    QStringList dirs = dir.entryList(QDir::Dirs);
    ui->comboBox->clear();
    ui->comboBox->addItems(dirs);
    ui->comboBox->setCurrentIndex(0);
}

void HostFileSystem::on_treeView_doubleClicked(const QModelIndex &index)
{
    QFileInfo fileinfo(model->filePath(index));
    if(fileinfo.isFile()){
        openFile(model->filePath(index));
    }
    else if (fileinfo.isDir()){
        on_comboBox_activated(model->filePath(index));
    }
}

void HostFileSystem::on_comboBox_activated(const QString &arg1)
{
    QDir dir;

    QString path = dir.cleanPath(dir.absoluteFilePath(arg1));
    model->setRootPath(path);
    dir.setCurrent(model->rootPath());
    ui->treeView->setRootIndex(model->index(path));
    ui->label->setText(QDir::cleanPath(path));
    fillDirBox();
}

void HostFileSystem::contextMenuEvent(QContextMenuEvent *e){
    Q_UNUSED(e);
    qDebug() <<"hola";
}

void HostFileSystem::on_pushButton_clicked()
{
    QString fileName;
    SimpleFileDialog sfd(&fileName,"Nuevo Archivo");
    if(sfd.exec()){
        if(!QFile(fileName).exists()){
            QFile f(fileName);
            f.open(QIODevice::WriteOnly);
            f.close();
        }
    }
}

void HostFileSystem::on_pushButton_2_clicked()
{
    QString dirname;
    SimpleFileDialog sfd(&dirname,"Nueva Carpeta");
    if(sfd.exec()){
        if(!QDir(dirname).exists())
            QDir().mkdir(dirname);
    }
}
void HostFileSystem::keyPressEvent(QKeyEvent *e){
    if (e->key() == Qt::Key_Delete){
        QString file = model->filePath(ui->treeView->currentIndex());
        if(QDir(file).exists())
            QDir(file).removeRecursively();
        else if (QFile(file).exists())
            QFile(file).remove();
    }
}
