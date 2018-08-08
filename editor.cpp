#include "editor.h"
#include "ui_editor.h"
#include <Qsci/qscilexerpython.h>
#include <Qsci/qsciscintilla.h>
#include <QDebug>

Editor::Editor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Editor)
{
    ui->setupUi(this);
    openedFiles = new QStringList();
}

Editor::~Editor()
{
    delete ui;
}

int Editor::addTab(QWidget *widget, QString string){
    ui->tabWidget->addTab(widget,string);
    ui->tabWidget->setCurrentWidget(widget);
    EditorTab* et = dynamic_cast<EditorTab*>(ui->tabWidget->currentWidget());
    //connect(et,SIGNAL(saveTargetFile(QString,QByteArray)),this,SIGNAL(saveTargetFile(QString,QByteArray)));
    connect(et,SIGNAL(content_changed(bool)),this,SLOT(currentTabContentChanged(bool)));
    return 0;
}
QString Editor::getCurrentTabText(){
    return ui->tabWidget->tabText(ui->tabWidget->currentIndex());
}
void Editor::setCurrentTabText(QString text){
    ui->tabWidget->setTabText(ui->tabWidget->currentIndex(),text);
}
void Editor::currentTabContentChanged(bool ch){
    QString ctxt = getCurrentTabText();
    if (ch){
        if(!ctxt.endsWith("*"))
            setCurrentTabText(ctxt+"*");
    }else{
        if(getCurrentTabText().endsWith("*"))
                setCurrentTabText(ctxt.remove(ctxt.length()-1,1));
    }
}
QTabWidget *Editor::getTabWidget(){
    return ui->tabWidget;
}
void Editor::openFile(QString filePath){
    EditorTab *et;
    for (int i=0; i<ui->tabWidget->count();i++){
        et = dynamic_cast<EditorTab*>(ui->tabWidget->widget(i));
        if( et->getFilePath() == filePath && et->is_host_file()){
            ui->tabWidget->setCurrentIndex(i);
            break;
        }
    }
    if (et->getFilePath() != filePath || !et->is_host_file()){
        addTab(new EditorTab(filePath),filePath);
        et = dynamic_cast<EditorTab*>(ui->tabWidget->currentWidget());
        connect(et,SIGNAL(saveTargetFile(QString,QByteArray)),this,SIGNAL(saveTargetFile(QString,QByteArray)));
    }
}

void Editor::openTargetFile(QString filePath, QString content){
    EditorTab *et;
    for (int i=0; i<ui->tabWidget->count();i++){
        et = dynamic_cast<EditorTab*>(ui->tabWidget->widget(i));
        if( et->getFilePath() == filePath && !(et->is_host_file()) ){
            ui->tabWidget->setCurrentIndex(i);
            break;
        }
    }
    if (et->getFilePath() != filePath || et->is_host_file()){
        addTab(new EditorTab(false),"(target)"+filePath);
        et = dynamic_cast<EditorTab*>(ui->tabWidget->currentWidget());
        connect(et,SIGNAL(saveTargetFile(QString,QByteArray)),this,SIGNAL(saveTargetFile(QString,QByteArray)));
        et->ed->setText(content);
        et->set_current_content(content);
        et->setLexer(filePath);
        et->setFilePath(filePath);

        connect(et->ed,SIGNAL(textChanged()),et,SLOT(on_content_changed()));
    }
}

EditorTab *Editor::getCurrentTab(){
    return dynamic_cast<EditorTab*>(ui->tabWidget->currentWidget());
}

void Editor::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->removeTab(index);
    if (index)
        ui->tabWidget->setCurrentIndex(index-1);
}
