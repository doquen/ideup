#include "console.h"
#include <QtConcurrent/QtConcurrent>
#include <QScrollBar>
#include <QClipboard>
#include <QKeySequence>
#include <QtCore/QDebug>
#include <QKeyEvent>
#include <QGuiApplication>
#include <QMenu>

Console::Console(QWidget *parent)
    : QTextEdit(parent)
    , localEchoEnabled(false)
{
    pos = 0;
    document()->setMaximumBlockCount(100);
    this->setEnabled(true);

}

void Console::putData(const QByteArray &data)
{
    int aux;
    bool ok;
    char n;
    QTextCursor cursor = textCursor();
    cursor.setPosition(pos);
    setTextCursor(cursor);
    qDebug() << data;
    for (int i=0; i< data.length(); i++){
        switch (data.at(i)){
        case 0x08:
            moveCursor(QTextCursor::Left);
            break;
        case 0x1b:
            i = i+2;
            aux = 0;
            ok = false;
            n = QString(QChar(data.at(i)).toLatin1()).toInt(&ok,10);
            while(ok){
                aux = aux*10+n;
                i++;
                n = QString(QChar(data.at(i)).toLatin1()).toInt(&ok,10);
            }
            switch (data.at(i)) {
            case 0x4b:
                moveCursor(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
                this->textCursor().removeSelectedText();
                break;
            case 0x44:
                QTextCursor cursor = textCursor();
                cursor.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,aux);
                setTextCursor(cursor);
                break;
            }
            break;
        default:
            this->textCursor().deleteChar();
            insertPlainText(QString(data.at(i)));
            QCoreApplication::processEvents();
        }
    }

    pos = textCursor().position();
    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::setLocalEchoEnabled(bool set)
{
    localEchoEnabled = set;
}

void Console::keyPressEvent(QKeyEvent *e)
{
    QByteArray ar;
    switch (e->key()) {
    case Qt::Key_Left:
        emit getData("[D");
        break;
    case Qt::Key_Right:
        emit getData("[C");
        break;
    case Qt::Key_Up:
        emit getData("[A");
        break;
    case Qt::Key_Down:
        emit getData("[B");
        break;
    default:
        if(e->key() == Qt::Key_V && (e->modifiers() & Qt::ControlModifier) && (e->modifiers() & Qt::ShiftModifier)){
            QClipboard *clipboard = QGuiApplication::clipboard();
            QString text = clipboard->text();
            ar.append(text);
            qDebug() << ar;
        }
        else if(e->key() == Qt::Key_C && (e->modifiers() & Qt::ControlModifier) && (e->modifiers() & Qt::ShiftModifier)){
            QClipboard *clipboard = QGuiApplication::clipboard();
            clipboard->setText(textCursor().selectedText());
        }
        else if(e->key() == Qt::Key_L && (e->modifiers() & Qt::ControlModifier)){
            QStringList list = toPlainText().split('\n');
            QString last = list.at(list.length()-1);
            clear();
            insertPlainText(last);
        }
        else{
            if (e->key()==Qt::Key_Enter || e->key() == Qt::Key_Return){
                moveCursor(QTextCursor::End);
                pos = (textCursor().position());
            }
            if (localEchoEnabled)
                QTextEdit::keyPressEvent(e);
            ar.append(e->text().toLatin1());
        }
        emit getData(ar);
    }
}

//void Console::mousePressEvent(QMouseEvent *e)
//{
//    Q_UNUSED(e)
//    setFocus();
//}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    //    QMenu *menu = new QMenu(this);
    //    menu->addAction("Copiar");
    //    menu->addAction("Pegar");

    //    menu->exec(e->globalPos());
    //    delete menu;
    Q_UNUSED(e)
}
