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

public slots:
    void addFiles(QStringList items, QStringList icons);
    void on_current_dir_changed(QString dir);

private slots:
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_comboBox_activated(const QString &arg1);

private:
    Ui::TargetFileSystem *ui;
    QString currentPath;
};

#endif // TARGETFILESYSTEM_H
