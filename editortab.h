#ifndef EDITORTAB_H
#define EDITORTAB_H

#include <QWidget>
#include <Qsci/qsciscintilla.h>

class EditorTab : public QWidget
{
    Q_OBJECT
public:
    explicit EditorTab(bool hostfile = true,QWidget *parent = nullptr);
    explicit EditorTab(QString filePath,bool hostfile = true,QWidget *parent = nullptr);
    ~EditorTab();
    QsciScintilla *ed;
    void setFilePath(QString filePath);
    void setLexer(QString filePath);
    QString getFilePath();
    void saveFile(QString path);
    bool is_host_file();
    void set_host_file(bool host);

private:
    QString mFilePath;
    bool hostfile;
    QFile *mFile;
    void openFile(QString path);

signals:
    void saveTargetFile(QString path, QByteArray content);
public slots:

};

#endif // EDITORTAB_H
