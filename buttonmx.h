///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file
/// @brief
/// @author
/// @date
/// @version  1.0
/// @note
///////////////////////////////////////////////////////
#ifndef BUTTONMX_H
#define BUTTONMX_H

#include <QPushButton>

class ButtonMx : public QPushButton
{
  Q_OBJECT
public:
  ButtonMx(QString title,QWidget *parent = 0);
  void setColorCustom(bool have);
  void setColor(const QString& color);
  void setChecked(bool isCheck);
  QSize sizeHint()const;
protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);
  void enterEvent(QEvent *event);
  void leaveEvent(QEvent *event);
signals:
  void chooseSIG();
public slots:
  void onChoose();
private:
  bool m_bHave;
  QString m_title;
  QString m_color;
  QKeySequence m_key;
};

#endif // BUTTONMX_H
