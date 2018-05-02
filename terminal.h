#ifndef TERMINAL_H
#define TERMINAL_H

#include <QWidget>
#include <QSerialPort>
#include <QMutex>
#include <QFileSystemModel>

namespace Ui {
class Terminal;
}

class Terminal : public QWidget
{
    Q_OBJECT

public:
    explicit Terminal(QWidget *parent = 0);
    ~Terminal();
    QString get_target_current_dir();
signals:
    void connected(QString portName, bool state = false);
    void sendScannedTargetFiles(QStringList items, QStringList icons);
    void update_target_dir(QString dir);
    void port_connected(bool c);
    void targetFileOpened(QString file, QString content);
public slots:
    void openTargetFile(QString file);
    void writeData(const QByteArray &data);
    void chdir(QString dir);
    void saveTargetFile(QString path, QByteArray content);
    void createTargetDir(QString path);
    void transferFileToTarget(QString fileName, QByteArray content);
    void deleteTargetFile(QString fileName);
    void deleteTargetDir(QString dirName);
    QString pwd();
private slots:
    void on_pushButton_clicked();
    void readData();
    void on_pushButton_3_clicked();
    void scan_ports();
    void scanTargetFileSystem();
    void on_pushButton_2_clicked();
    void targetDirChanged(QString dir);

private:
    QSerialPort *port;
    int readDelay;
    Ui::Terminal *ui;
    QMutex mutex;
    bool toConsole;
    QByteArray internalData;
    QByteArray auxInternalData;
    QFileSystemModel *tfs;
    QString targetCurrentDir;
};

#endif // TERMINAL_H
