#ifndef SIMPLEFILEDIALOG_H
#define SIMPLEFILEDIALOG_H

#include <QDialog>

namespace Ui {
class SimpleFileDialog;
}

class SimpleFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SimpleFileDialog(QWidget *parent = 0);
    explicit SimpleFileDialog(QString *name, QString title, bool saveAs = false, QWidget *parent = 0);
    ~SimpleFileDialog();
    bool host_selected();

private slots:
    void on_lineEdit_textEdited(const QString &arg1);

    void on_buttonBox_accepted();

private:
    Ui::SimpleFileDialog *ui;
    QString *o_name;
    QString m_name;
    QString m_title;
};

#endif // SIMPLEFILEDIALOG_H
