#ifndef HOSTFILESYSTEM_H
#define HOSTFILESYSTEM_H

#include <QWidget>
#include <QFileSystemModel>

namespace Ui {
class HostFileSystem;
}

class HostFileSystem : public QWidget
{
    Q_OBJECT
signals:
    void openFile(QString filePath);
public:
    explicit HostFileSystem(QWidget *parent = 0);
    ~HostFileSystem();

private slots:
    void on_treeView_doubleClicked(const QModelIndex &index);

    void fillDirBox();

    void on_comboBox_activated(const QString &arg1);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::HostFileSystem *ui;
    QFileSystemModel *model;

protected:
    void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
};

#endif // HOSTFILESYSTEM_H
