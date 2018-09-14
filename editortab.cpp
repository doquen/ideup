#include "editortab.h"
#include <QGridLayout>
#include <Qsci/qscilexerpython.h>
#include <Qsci/qscilexerhtml.h>
#include <Qsci/qscilexerjavascript.h>
#include <QFontDatabase>
#include <Qsci/qsciapis.h>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTabWidget>


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
    ed->setIndentationsUseTabs(false);
    ed->setIndentationWidth(4);
    ed->setMarginLineNumbers(1,true);
    ed->setMarginWidth(1,30);
    ed->setAutoCompletionCaseSensitivity(false);
    ed->setAutoCompletionThreshold(2);
    ed->setAutoCompletionFillupsEnabled(true);
    ed->setAutoCompletionSource(QsciScintilla::AcsAll);
    ed->setEdgeMode(QsciScintilla::EdgeLine);
    ed->setEdgeColumn(80);
    ed->setEdgeColor(QColor(Qt::lightGray));

//    QsciAPIs * api = new QsciAPIs(lexer);
//    if (!api->load("microPython.api"))
//        qDebug() << "false";
//    api->add("asdf");
//    api->prepare();

    QFont font = QFont("Monospace",12);
    font.setStyleHint(QFont::Monospace);
    lexer->setFont(font);
    ed->setLexer(lexer);

    gridLayout->addWidget(ed);

    mFile = new QFile();

    mFileContent = "";

}
void EditorTab::set_current_content(QString content){
    QByteArray aux;
    aux.clear();
    mFileContent = aux.append(content);
}
void EditorTab::setLexer(QString filePath){
    if(filePath.endsWith(".py")){
        ed->setLexer(new QsciLexerPython());
    }
    else if(filePath.endsWith(".html"))
        ed->setLexer(new QsciLexerHTML());
    else if(filePath.endsWith(".js"))
        ed->setLexer(new QsciLexerJavaScript());

    QFont font = QFont("Monospace",12);
    font.setStyleHint(QFont::Monospace);
    if(ed->lexer())
        ed->lexer()->setFont(font);
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
    connect(ed,SIGNAL(textChanged()),this,SLOT(on_content_changed()));
}

void EditorTab::on_content_changed(){
    QString edtxt = ed->text();
    content_changed(ed->text()!=QString(mFileContent));
}

void EditorTab::set_host_file(bool host){
    hostfile = host;
}
void EditorTab::openFile(QString path){
    delete mFile;
    mFile = new QFile(path);
    mFile->open(QIODevice::ReadOnly);
    mFileContent = mFile->readAll();
    ed->setText(mFileContent);
    mFile->close();
}

void EditorTab::saveFile(QString path){
    mFileContent.clear();
    mFileContent.append(ed->text());
    if(hostfile){
        delete mFile;
        mFile = new QFile(path);
        mFile->open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
        mFile->write(ed->text().toUtf8());
        mFile->close();
        mFilePath = QDir().absoluteFilePath(path);
    }else{
//        QByteArray lines;
//        for (int i=0; i<ed->lines();i++)
//            lines.append(ed->text());
        QString a = QString(ed->text());
        saveTargetFile(path,ed->text().toUtf8());
        mFilePath = path;
    }
    on_content_changed();
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
