///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file
/// @brief
/// @author
/// @date
/// @version  1.0
/// @note
///////////////////////////////////////////////////////
#ifndef DIALOGMX_H
#define DIALOGMX_H

#include <QDialog>

class DialogMx : public QDialog
{
  Q_OBJECT
public:
  explicit DialogMx(QWidget *parent = 0);
  ~DialogMx();
protected:
  // 拖拽界面
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *event);
private:
  QPoint      m_curPos;
};

#endif // DIALOGMX_H
