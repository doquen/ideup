#include "editortab.h"
#include <QGridLayout>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexerjavascript.h>
#include <Qsci/qsciapis.h>
#include <QDebug>
#include <QDir>
#include <QFileInfo>


EditorTab::EditorTab(bool hostfile, QWidget *parent) : QWidget(parent),
    hostfile(hostfile)
{
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(0,0,0,0);
    gridLayout->setObjectName(QStringLiteral("gridLayout"));

    ed = new QsciScintilla();
    QsciLexer * lexer = new QsciLexerPython();

    ed->setAutoIndent(true);
    ed->setMarginLineNumbers(1,true);
    ed->setAutoCompletionCaseSensitivity(false);
    ed->setAutoCompletionThreshold(2);
    ed->setAutoCompletionFillupsEnabled(true);
    ed->setAutoCompletionSource(QsciScintilla::AcsAll);

//    QsciAPIs * api = new QsciAPIs(lexer);
//    if (!api->load("microPython.api"))
//        qDebug() << "false";
//    api->add("asdf");
//    api->prepare();

    ed->setLexer(lexer);

    gridLayout->addWidget(ed);

    mFile = new QFile();

}

void EditorTab::setLexer(QString filePath){
    if(filePath.endsWith(".py")){
        ed->setLexer(new QsciLexerPython());
    }
    else if(filePath.endsWith(".html"))
        ed->setLexer(new QsciLexerHTML());
    else if(filePath.endsWith(".js"))
        ed->setLexer(new QsciLexerJavaScript());
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
    ed->setMarginLineNumbers(1,true);
    setLexer(filePath);
    ed->setAutoIndent(true);
    gridLayout->addWidget(ed);

    mFile = new QFile();

    openFile(filePath);
}

void EditorTab::set_host_file(bool host){
    hostfile = host;
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
        mFilePath = QDir().absoluteFilePath(path);
    }else{
//        QByteArray lines;
//        for (int i=0; i<ed->lines();i++)
//            lines.append(ed->text());
        QString a = QString(ed->text());
        saveTargetFile(path,ed->text().toLatin1());
        mFilePath = path;
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
