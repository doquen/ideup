#ifndef TARGETFILESYSTEM_H
#define TARGETFILESYSTEM_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class TargetFileSystem;
}

class TargetFileSystem : public QWidget
{
    Q_OBJECT

public:
    explicit TargetFileSystem(QWidget *parent = 0);
    ~TargetFileSystem();

signals:
    void currentDirChanged(QString newPath);
    void openFile(QString file);
    void createNewTargetFile(QString name, QByteArray content);
    void createNewTargetDir(QString name);
    void deleteTargetFile(QString name);
    void deleteTargetDir(QString name);

public slots:
    void addFiles(QStringList items, QStringList icons);
    void on_current_dir_changed(QString dir);

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_comboBox_activated(const QString &arg1);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::TargetFileSystem *ui;
    QString currentPath;
protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
};

#endif // TARGETFILESYSTEM_H
