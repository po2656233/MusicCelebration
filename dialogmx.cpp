///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file
/// @brief
/// @author
/// @date
/// @version  1.0
/// @note
///////////////////////////////////////////////////////
#include "dialogmx.h"
#include <QMouseEvent>
#include <QApplication>
#include <windows.h>
#include <windowsx.h>
DialogMx::DialogMx(QWidget *parent) :
  QDialog(parent)
{
  m_curPos = QPoint(0,0);
}

DialogMx::~DialogMx()
{

}

void DialogMx::mousePressEvent(QMouseEvent *event)
{
  if (Qt::LeftButton == event->button() || Qt::RightButton == event->button())
    {
      m_curPos = event->globalPos() - pos();
      event->accept();
    }
  event->ignore();
  return QDialog::mousePressEvent(event);
}

void DialogMx::mouseMoveEvent(QMouseEvent *event)
{
  if (m_curPos != QPoint(0,0)) move( event->globalPos() - m_curPos);
  event->accept();
  return QDialog::mouseMoveEvent(event);
}

void DialogMx::mouseReleaseEvent(QMouseEvent *event)
{
  m_curPos = QPoint(0,0);
  QApplication::restoreOverrideCursor();
  event->ignore();
  QWidget::mouseReleaseEvent(event);
}


