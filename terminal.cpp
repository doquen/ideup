#include "terminal.h"
#include "ui_terminal.h"
#include "console.h"
#include <QSerialPortInfo>
#include <QList>
#include <QDebug>
#include <QThread>
#include <QRegularExpression>

Terminal::Terminal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Terminal)
{
    ui->setupUi(this);
    scan_ports();
    port = new QSerialPort();
    toConsole = true;
    targetCurrentDir = "/";
}

Terminal::~Terminal()
{
    delete ui;
}

void Terminal::scan_ports(){
    QList<QSerialPortInfo> *infos = new QList<QSerialPortInfo>(QSerialPortInfo::availablePorts());
    for(int i=0; i< infos->length(); i++){
        ui->comboBox->addItem(infos->at(i).portName());
    }
}
void Terminal::on_pushButton_clicked()
{
    if(!ui->pushButton->isChecked()){
        ui->pushButton->setText("Conectar");
        port->close();
        disconnect(port,SIGNAL(readyRead()),this,SLOT(readData()));
        disconnect(ui->textEdit,SIGNAL(getData(QByteArray)),this,SLOT(writeData(QByteArray)));
        //update_target_dir("");
        port_connected(false);
    }
    else{
        ui->pushButton->setText("Desconectar");
        delete port;
        port = new QSerialPort(ui->comboBox->currentText());
        port->setBaudRate(ui->comboBox_2->currentText().toInt());

        if(port->open(QIODevice::ReadWrite)){
            connect(port,SIGNAL(readyRead()),this,SLOT(readData()),Qt::DirectConnection);
            connect(ui->textEdit,SIGNAL(getData(QByteArray)),this,SLOT(writeData(QByteArray)),Qt::DirectConnection);
            this->connected(port->portName(),true);
            targetCurrentDir = pwd();
            update_target_dir(targetCurrentDir);
            scanTargetFileSystem();
            port_connected(true);
        }
        else{
            this->connected(port->portName(),false);
            ui->pushButton->setChecked(false);
            ui->pushButton->setText("Conectar");
            port_connected(false);
        }
    }

}
void Terminal::readData()
{
    QByteArray data;
    data.clear();
    if(port->isOpen()){
        do{
            data.append(port->readAll());
            QCoreApplication::processEvents();
        }while(port->waitForReadyRead(20));
        if(toConsole)
            ui->textEdit->putData(data);
        else{
            toConsole = true;
            internalData = data;
        }
    }
}

void Terminal::writeData(const QByteArray &data)
{
    if(port->isOpen())
        port->write(data);
}

void Terminal::on_pushButton_3_clicked()
{
    ui->textEdit->clear();
}

void Terminal::on_pushButton_2_clicked()
{
    scan_ports();
}

void Terminal::scanTargetFileSystem(){
    QByteArray data;

    int lastPos=0;
    QStringList dirList;
    QStringList dirs;
    QStringList icons;
    toConsole = false;
    data.append(0x05);
    data.append("import os\n");
    data.append("IDEUPdirs = os.listdir()\n");
    data.append("IDEUPdirsicons = []\n");
    data.append("for i in IDEUPdirs:\n");
    data.append("\tIDEUPdirsicons.append(i)\n");
    data.append("\tif os.stat(i)[0] & 0o170000 == 0o040000:\n");
    data.append("\t\tIDEUPdirsicons.append('dir')\n");
    data.append("\telse:\n");
    data.append("\t\tIDEUPdirsicons.append('file')\n");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    qDebug() << internalData;
    toConsole = false;
    data.clear();
    data.append(0x05);
    data.append("print(IDEUPdirsicons)");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    internalData.remove(0,internalData.indexOf("[")+1);
    internalData.remove(internalData.indexOf("]"),
                        internalData.length()-internalData.indexOf("]"));
    while(lastPos!=-1){
        dirList.append(
                    internalData.mid(lastPos+1,
                                     internalData.indexOf("'",lastPos+1)-lastPos-1));
        lastPos=internalData.indexOf("\'",lastPos+1);
        lastPos=internalData.indexOf("\'",lastPos+1);
    }
    qDebug() << dirList;

    if(dirList.length()>1)
        for(int i=0; i < dirList.length(); i=i+2){
            dirs.append(dirList.at(i));
            icons.append(dirList.at(i+1));
        }

    sendScannedTargetFiles(dirs,icons);

    toConsole = false;
    data.clear();
    data.append(0x05);
    data.append("del IDEUPdirs\n");
    data.append("del IDEUPdirsicons\n");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
}

void Terminal::targetDirChanged(QString dir){
    chdir(QDir::cleanPath(targetCurrentDir+"/"+dir));
    targetCurrentDir = pwd();
    update_target_dir(targetCurrentDir);
    scanTargetFileSystem();
}

void Terminal::chdir(QString dir){
    QByteArray data;
    toConsole = false;
    data.clear();
    data.append(0x05);
    data.append("import os\n");
    data.append("os.chdir('"+dir+"')");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
}

QString Terminal::pwd(){
    QByteArray data;
    toConsole = false;
    data.clear();
    data.append(0x05);
    data.append("import os\n");
    data.append("print('['+os.getcwd()+']')");
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    toConsole=false;
    data.clear();
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    qDebug()<<internalData.mid(internalData.indexOf("[")+1,internalData.indexOf("]")-internalData.indexOf("[")-1);
    return internalData.mid(internalData.indexOf("[")+1,internalData.indexOf("]")-internalData.indexOf("[")-1);
}

void Terminal::openTargetFile(QString file){
    QByteArray data;
    toConsole = false;
    data.clear();
    data.append(0x05);
    data.append("IDEUPfile = open('"+file+"','r')\n");
    data.append("IDEUPcontent = IDEUPfile.read()\n");
    data.append("IDEUPfile.close()\n");
    data.append("print(IDEUPcontent)");
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    toConsole=false;
    data.clear();
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    internalData.remove(internalData.indexOf("\r\n"),2);
    internalData.remove(internalData.indexOf("\r\n>>>"),5);
    targetFileOpened(QDir::cleanPath(targetCurrentDir+"/"+file),internalData);
    toConsole=false;
    data.clear();
    data.append(0x05);
    data.append("del IDEUPfile\n");
    data.append("del IDEUPcontent\n");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
}

void Terminal::saveTargetFile(QString path, QByteArray content){
    QByteArray data;
    toConsole = false;
    data.clear();
    data.append(0x05);
    data.append("IDEUPfile = open('"+path+"','w')\n");
    data.append("IDEUPfile.write('"+content+"')\n");
    data.append("IDEUPfile.close()\n");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
}
