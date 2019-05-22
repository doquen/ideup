#ifndef CONSOLE_H
#define CONSOLE_H

#include <QTextEdit>

class Console : public QTextEdit
{
    Q_OBJECT

signals:
    void getData(const QByteArray &data);
    void reset();

public:
    explicit Console(QWidget *parent = nullptr);
    void putData(const QByteArray &data);
    void setLocalEchoEnabled(bool set);
    bool reseted;

protected:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
//    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void contextMenuEvent(QContextMenuEvent *e) Q_DECL_OVERRIDE;

private:
    bool localEchoEnabled;
    int pos;

};

#endif // CONSOLE_H
