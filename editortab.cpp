#include "editortab.h"
#include <QGridLayout>
#include <Qsci/qscilexerpython.h>
#include <QDebug>

EditorTab::EditorTab(bool hostfile, QWidget *parent) : QWidget(parent),
    hostfile(hostfile)
{
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setObjectName(QStringLiteral("gridLayout"));

    ed = new QsciScintilla(this);
    ed->setLexer(new QsciLexerPython());
    ed->setAutoIndent(true);
    gridLayout->addWidget(ed);

    mFile = new QFile();

}

EditorTab::EditorTab(QString filePath, bool hostfile, QWidget *parent) : QWidget(parent),
    mFilePath(filePath),
    hostfile(hostfile)
{
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setObjectName(QStringLiteral("gridLayout"));

    ed = new QsciScintilla(this);
    ed->setLexer(new QsciLexerPython());
    ed->setAutoIndent(true);
    gridLayout->addWidget(ed);

    mFile = new QFile();

    openFile(filePath);
}
void EditorTab::openFile(QString path){
    delete mFile;
    mFile = new QFile(path);
    mFile->open(QIODevice::ReadOnly);

    ed->setText(mFile->readAll());

    mFile->close();
}

void EditorTab::saveFile(QString path){
    if(hostfile){
        delete mFile;
        mFile = new QFile(path);
        mFile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        mFile->write(ed->text().toLatin1());
        mFile->close();
    }else{
        saveTargetFile(mFilePath,ed->text().toLocal8Bit());
    }
}
EditorTab::~EditorTab(){
}

void EditorTab::setFilePath(QString filePath){
    mFilePath = filePath;
}

QString EditorTab::getFilePath(){
    return mFilePath;
}

bool EditorTab::is_host_file(){
    return hostfile;
}
