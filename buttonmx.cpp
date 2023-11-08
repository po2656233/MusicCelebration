///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file
/// @brief
/// @author
/// @date
/// @version  1.0
/// @note
///////////////////////////////////////////////////////
#include "buttonmx.h"
#include <QMouseEvent>

ButtonMx::ButtonMx(QString title, QWidget *parent) :
  QLabel(parent),m_bHave(false),m_title(title),m_color("")
{
  //this->setStyleSheet("background-color: #228B22;");
  this->setText(m_title);
  this->setAlignment(Qt::AlignCenter);
  connect(this, SIGNAL(chooseSIG()), this, SLOT(onChoose()));
}

void ButtonMx::setColorCustom(bool have)
{
  m_bHave = have;
}

void ButtonMx::setColor(const QString &color)
{
  m_color = color;
  this->setStyleSheet(m_color);
}


void ButtonMx::setChecked(bool isCheck)
{
  if(isCheck) emit chooseSIG();
}

QSize ButtonMx::sizeHint() const
{
  return QSize(100,30);
}

void ButtonMx::mousePressEvent(QMouseEvent *event)
{
  if (Qt::LeftButton == event->button()) emit chooseSIG();
}

void ButtonMx::mouseReleaseEvent(QMouseEvent *event)
{
  Q_UNUSED(event)
  this->setStyleSheet(m_color);
}

void ButtonMx::enterEvent(QEvent *)
{
  m_bHave?this->setStyleSheet("background-color: #4B0082;"):this->setStyleSheet("background-color: #7FFF00;");
}

void ButtonMx::leaveEvent(QEvent *)
{
  m_bHave?this->setStyleSheet(m_color):this->setStyleSheet("background-color: #DDA0DD;");
}

void ButtonMx::onChoose()
{
  this->setStyleSheet("background-color: #7FFF00;");
  //m_bHave?this->setStyleSheet("background-color: #F5FFFA;"):this->setStyleSheet("background-color: #7FFF00;");
}
