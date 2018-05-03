#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include <QToolBar>
#include <QTabWidget>
#include "editortab.h"


namespace Ui {
class Editor;
}

class Editor : public QWidget
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = 0);
    ~Editor();
    int addTab(QWidget* widget, QString string);
    EditorTab *getCurrentTab();
    QTabWidget *getTabWidget();
signals:
    void saveTargetFile(QString path, QByteArray content);
public slots:
    void openFile(QString filePath);
    void openTargetFile(QString filePath, QString content);
private slots:
    void on_tabWidget_tabCloseRequested(int index);

private:
    Ui::Editor *ui;
    QString filePath;
    QFile file;
};

#endif // EDITOR_H
