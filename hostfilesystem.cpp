#include "hostfilesystem.h"
#include "ui_hostfilesystem.h"
#include <QFileSystemModel>
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
    dir.setCurrent(model->rootPath());
    QString path = dir.cleanPath(dir.absoluteFilePath(arg1));
    model->setRootPath(path);
    ui->treeView->setRootIndex(model->index(path));
    ui->label->setText(QDir::cleanPath(path));
    fillDirBox();
}

void HostFileSystem::contextMenuEvent(QContextMenuEvent *e){
    qDebug() <<"hola";
}
