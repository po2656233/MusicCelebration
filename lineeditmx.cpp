#include "lineeditmx.h"
#include<QKeyEvent>

LineEditMx::LineEditMx(QWidget *parent):QLineEdit(parent)
{
    this->setFocus();
}

LineEditMx::~LineEditMx()
{

}


void LineEditMx::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Escape/* || event->key()==Qt::Key_Return || event->key()==Qt::Key_Enter*/){
        emit over();
    }
    event->ignore();
    QLineEdit::keyPressEvent(event);
}

void LineEditMx::focusOutEvent(QFocusEvent *e)
{
//    emit over();
    QLineEdit::focusOutEvent(e);
}
