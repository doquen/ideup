#include "targetfilesystem.h"
#include "ui_targetfilesystem.h"

#include <QFileIconProvider>

TargetFileSystem::TargetFileSystem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TargetFileSystem)
{
    ui->setupUi(this);
    currentPath = "/";
    this->setEnabled(false);
}

TargetFileSystem::~TargetFileSystem()
{
    delete ui;
}

void TargetFileSystem::addFiles(QStringList items, QStringList icons){
    QFileIconProvider ip;
    ui->listWidget->clear();
    ui->listWidget->addItems(items);
    ui->comboBox->clear();
    ui->comboBox->addItem(".");
    ui->comboBox->addItem("..");
    for(int i=0; i<ui->listWidget->count(); i++){
        if(icons.at(i) == "file"){
            ui->listWidget->item(i)->setIcon(ip.icon(QFileIconProvider::File));
            ui->listWidget->item(i)->setData(Qt::UserRole,"file");
        }
        else{
            ui->listWidget->item(i)->setIcon(ip.icon(QFileIconProvider::Folder));
            ui->listWidget->item(i)->setData(Qt::UserRole,"dir");
            ui->comboBox->addItem(items.at(i));
        }
    }
}

void TargetFileSystem::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if(item->data(Qt::UserRole)=="dir"){
        currentDirChanged(item->text());
    }else{
        openFile(item->text());
    }
}

void TargetFileSystem::on_comboBox_activated(const QString &arg1)
{
        currentDirChanged(arg1);
}

void TargetFileSystem::on_current_dir_changed(QString dir){
    currentPath = dir;
    ui->label->setText(dir);
}
