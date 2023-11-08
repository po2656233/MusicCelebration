///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file
/// @brief
/// @author
/// @date
/// @version  1.0
/// @note
///////////////////////////////////////////////////////
#ifndef SPEEDCONTROL_H
#define SPEEDCONTROL_H

#include <QLabel>
class ButtonMx;
class SpeedControl : public QLabel
{
  Q_OBJECT
public:
  SpeedControl(QWidget *parent = 0);
signals:
  void sigSlowDown();
  void sigQuickUp();
  void sigRecover();
protected:
  void resizeEvent(QResizeEvent *sizeEvt);
private:
  void layBtn();
  ButtonMx* m_slowDownBtn;
  ButtonMx* m_recoverBtn;
  ButtonMx* m_quickUpBtn;
};

#endif // SPEEDCONTROL_H
