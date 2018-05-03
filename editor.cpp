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
}

Editor::~Editor()
{
    delete ui;
}

int Editor::addTab(QWidget *widget, QString string){
    ui->tabWidget->addTab(widget,string);
    ui->tabWidget->setCurrentWidget(widget);
    EditorTab* et = (EditorTab*)(ui->tabWidget->currentWidget());
    connect(et,SIGNAL(saveTargetFile(QString,QByteArray)),this,SIGNAL(saveTargetFile(QString,QByteArray)));
    return 0;
}
QTabWidget *Editor::getTabWidget(){
    return ui->tabWidget;
}
void Editor::openFile(QString filePath){
    addTab(new EditorTab(filePath),filePath);
    EditorTab* et = (EditorTab*)(ui->tabWidget->currentWidget());
    connect(et,SIGNAL(saveTargetFile(QString,QByteArray)),this,SIGNAL(saveTargetFile(QString,QByteArray)));
}

void Editor::openTargetFile(QString filePath, QString content){
    addTab(new EditorTab(false),"(target)"+filePath);
    EditorTab* et = (EditorTab*)(ui->tabWidget->currentWidget());
    connect(et,SIGNAL(saveTargetFile(QString,QByteArray)),this,SIGNAL(saveTargetFile(QString,QByteArray)));
    et->ed->setText(content);
    et->setFilePath(filePath);
}

EditorTab *Editor::getCurrentTab(){
    return (EditorTab *)ui->tabWidget->currentWidget();
}

void Editor::on_tabWidget_tabCloseRequested(int index)
{
    ui->tabWidget->removeTab(index);
    if (index)
        ui->tabWidget->setCurrentIndex(index-1);
}
