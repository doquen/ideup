#include "simplefiledialog.h"
#include "ui_simplefiledialog.h"

SimpleFileDialog::SimpleFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimpleFileDialog)
{
    ui->setupUi(this);
    ui->widget->setVisible(false);
}

SimpleFileDialog::SimpleFileDialog(QString *name, QString title, bool saveAs, QWidget *parent) :
    QDialog(parent),
    o_name(name),
    m_title(title),
    ui(new Ui::SimpleFileDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    ui->widget->setVisible(saveAs);
}

SimpleFileDialog::~SimpleFileDialog()
{
    delete ui;
}

bool SimpleFileDialog::host_selected(){
    return ui->radioButton->isChecked();
}

void SimpleFileDialog::on_lineEdit_textEdited(const QString &arg1)
{
    m_name = arg1;
}

void SimpleFileDialog::on_buttonBox_accepted()
{
    *o_name = m_name;
}
