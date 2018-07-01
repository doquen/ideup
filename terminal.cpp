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
    readDelay = 20;
}

Terminal::~Terminal()
{
    delete ui;
}

void Terminal::scan_ports(){
    QList<QSerialPortInfo> *infos = new QList<QSerialPortInfo>(QSerialPortInfo::availablePorts());
    ui->comboBox->clear();
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

    if(port->isOpen()){
        while(port->bytesAvailable()){
            QByteArray data;
            do{
                data.append(port->readAll());
                QCoreApplication::processEvents();
            }while(port->waitForReadyRead(1));
            if(toConsole)
                ui->textEdit->putData(data);
            else{
                qDebug() << data;
                auxInternalData.append(data);
                if(auxInternalData.endsWith("EOIDEupC\r\n>>> ")){
                    internalData = auxInternalData;
                    auxInternalData.clear();
                    toConsole = true;
                }

            }
            data.clear();
        }
    }
}

void Terminal::writeData(const QByteArray &data)
{
    if(port->isOpen()){
        port->write(data);
    }
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
    cancelar();
    toConsole = false;
    data.append("\r");
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
    data.append("print('EOIDEupC')");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    toConsole = false;
    data.clear();
    data.append(0x05);
    data.append("print(IDEUPdirsicons)\n");
    data.append("print('EOIDEupC')");
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
    data.append("print('EOIDEupC')");
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
    cancelar();
    toConsole = false;
    data.clear();
    data.append("\r");
    data.append(0x05);
    data.append("import os\n");
    data.append("os.chdir('"+dir+"')\n");
    data.append("print('EOIDEupC')");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
}

QString Terminal::pwd(){
    QByteArray data;
    cancelar();
    toConsole = false;
    data.clear();
    data.append("\r");
    data.append(0x05);
    data.append("import os\n");
    data.append("IDEupCurrentDir = '['+ os.getcwd() +']'\n");
    data.append("print('EOIDEupC')");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    toConsole=false;
    data.clear();
    data.append("\r");
    data.append(0x05);
    data.append("print(IDEupCurrentDir)\n");
    data.append("del IDEupCurrentDir\n");
    data.append("print('EOIDEupC')");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    return internalData.mid(internalData.indexOf("[")+1,internalData.indexOf("]")-internalData.indexOf("[")-1);
}

void Terminal::openTargetFile(QString file){
    QByteArray data;
    QByteArray auxdata;
    int fileSize;
    auxdata.clear();
    cancelar();
    toConsole = false;
    data.clear();
    data.append("\r");
    data.append(0x05);
    data.append("import os\n");
    data.append("IDEUPfileSize = os.stat('"+file+"')[6]\n");
    data.append("IDEUPfile = open('"+file+"','r')\n");
    data.append("IDEUPfileSize = 'IDEUPfileSize: '+ str(IDEUPfileSize) + ' bytes'\n");
    data.append("print('EOIDEupC')");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    toConsole = false;
    data.clear();
    data.append(0x05);
    data.append("print(IDEUPfileSize)\n");
    data.append("print('EOIDEupC')");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    fileSize = QString(internalData.mid(internalData.indexOf("IDEUPfileSize: ")+15,
                                        internalData.indexOf(" bytes") -
                                        internalData.indexOf("IDEUPfileSize: ")-
                                        15)).toInt();
    do{
        toConsole = false;
        data.append(0x05);
        data.append("IDEUPcontent = IDEUPfile.read("+QString::number(READ_FILE_CHUNK_SIZE)+")\n");
        data.append("IDEupReadFileCommand = 'SOIDEupF'+IDEUPcontent+'EOIDEupF'\n");
        data.append("print('EOIDEupC')");
        data.append(0x04);
        writeData(data);
        while (!toConsole) {
            QCoreApplication::processEvents();
        }
        toConsole=false;
        data.clear();
        data.append(0x05);
        data.append("print(IDEupReadFileCommand)\n");
        data.append("print('EOIDEupC')");
        data.append(0x04);
        writeData(data);
        while (!toConsole) {
            QCoreApplication::processEvents();
        }
        internalData = internalData.mid(internalData.indexOf("SOIDEupF")+8,internalData.indexOf("EOIDEupF")-internalData.indexOf("SOIDEupF")-8);
        auxdata.append(internalData);

        update_file_status((int)(100.0*auxdata.size()/fileSize+0.5));
        QApplication::processEvents();

    } while(internalData != QByteArray(""));
    
    update_file_status(100);
    QApplication::processEvents();

    targetFileOpened(QDir::cleanPath(targetCurrentDir+"/"+file),auxdata);
    toConsole=false;
    data.clear();
    data.append(0x05);
    data.append("IDEUPfile.close()\n");
    data.append("del IDEUPfile\n");
    data.append("del IDEUPcontent\n");
    data.append("print('EOIDEupC')");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
}

void Terminal::saveTargetFile(QString path, QByteArray content){
    QByteArray data;
    QByteArray contentToWrite;
    cancelar();
    toConsole = false;
    readDelay=1000;

    contentToWrite = content.mid(0,WRITE_FILE_CHUNK_SIZE);
    contentToWrite.replace('\\',"\\\\");
    contentToWrite.replace(QChar('\n'),"\\n");
    contentToWrite.replace(QChar('\r'),"");
    contentToWrite.replace("'","\\'");
    contentToWrite.replace('"',"\\\"");
    data.clear();
    data.append("\r");

    data.append(0x05);
    data.append("import os\n");
    data.append("IDEUPfile = open('"+path+"','wb')\n");
    data.append("IDEUPfile.write(b'"+contentToWrite+"')\n");
    data.append("IDEUPfile.close()\n");
    data.append("print('EOIDEupC')");
    data.append(0x04);

    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    toConsole = false;
    for (int i=WRITE_FILE_CHUNK_SIZE;i<content.length();i+=WRITE_FILE_CHUNK_SIZE){
        update_file_status((int)(100.0*i/content.size()+0.5));
        QApplication::processEvents();
        contentToWrite = content.mid(i,WRITE_FILE_CHUNK_SIZE);
        contentToWrite.replace('\\',"\\\\");
        contentToWrite.replace(QChar('\n'),"\\n");
        contentToWrite.replace(QChar('\r'),"");
        contentToWrite.replace("'","\\'");
        contentToWrite.replace('"',"\\\"");
        data.clear();
        data.append(0x05);
        data.append("IDEUPfile = open('"+path+"','ab')\n");
        data.append("IDEUPfile.write(b'"+contentToWrite+"')\n");
        data.append("IDEUPfile.close()\n");
        data.append("print('EOIDEupC')");
        data.append(0x04);
        writeData(data);
        while (!toConsole) {
            QCoreApplication::processEvents();
        }
        toConsole = false;
    }
    update_file_status(100);
    data.clear();
    data.append(0x05);
    //data.append("del IDEUPfile\n");
    data.append("print('EOIDEupC')\n");
    data.append(0x04);
    writeData(data);

    while (!toConsole) {
        QCoreApplication::processEvents();
    }

    readDelay = 20;
    scanTargetFileSystem();
}

void Terminal::createTargetDir(QString path){
    QByteArray data;
    cancelar();
    toConsole = false;
    readDelay=175;
    data.clear();
    data.append("\r");
    data.append(0x05);
    data.append("import os\n");
    data.append("os.mkdir('"+path+"')\n");
    data.append("print('EOIDEupC')");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    readDelay = 20;
    scanTargetFileSystem();
}

void Terminal::transferFileToTarget(QString fileName, QByteArray content){
    QByteArray data;
    cancelar();
    toConsole = false;
    data.clear();
    data.append("\r");
    data.append(0x05);
    data.append("import os\n");
    data.append("IDEUPfile = open('"+fileName+"','w')\n");
    data.append("IDEUPfile.write('"+content+"')\n");
    data.append("IDEUPfile.close()\n");
    data.append("print('EOIDEupC')");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    scanTargetFileSystem();
}

void Terminal::deleteTargetDir(QString dirName){
    QByteArray data;
    cancelar();
    toConsole = false;
    data.clear();
    data.append("\r");
    data.append(0x05);
    data.append("import os\n");
    data.append("os.rmdir('"+dirName+"')\n");
    data.append("print('EOIDEupC')");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    scanTargetFileSystem();
}
void Terminal::deleteTargetFile(QString fileName){
    QByteArray data;
    cancelar();
    toConsole = false;
    data.clear();
    data.append("\r");
    data.append(0x05);
    data.append("import os\n");
    data.append("os.remove('"+fileName+"')\n");
    data.append("print('EOIDEupC')");
    data.append(0x04);
    writeData(data);
    while (!toConsole) {
        QCoreApplication::processEvents();
    }
    scanTargetFileSystem();
}
QString Terminal::get_target_current_dir(){
    return targetCurrentDir;
}

void Terminal::cancelar(){
    QByteArray data;
    data.clear();
    data.append("\u0003");
    writeData(data);
}
