#ifndef TERMINAL_H
#define TERMINAL_H

#define WRITE_FILE_CHUNK_SIZE 1000
#define READ_FILE_CHUNK_SIZE 1000
#define TIME_TO_WRITE 300

#include <QWidget>
#include <QSerialPort>
#include <QMutex>
#include <QFileSystemModel>
#include <QTimer>

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
    void update_file_status(int value);
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
    void cancelar();
    void on_pushButton_clicked();
    void readData();
    void on_pushButton_3_clicked();
    void scan_ports();
    void open_port();
    void close_port();
    void scanTargetFileSystem();
    void on_pushButton_2_clicked();
    void targetDirChanged(QString dir);
    void killCom();

private:
    QSerialPort *port;
    int readDelay;
    Ui::Terminal *ui;
    QMutex mutex;
    bool toConsole;
    bool kill;
    bool idle;
    QByteArray internalData;
    QByteArray auxInternalData;
    QFileSystemModel *tfs;
    QString targetCurrentDir;
    QTimer *toTimer;
};

#endif // TERMINAL_H
