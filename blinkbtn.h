///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file blinkbtn.h
/// @brief 文字发光
/// @author czh
/// @date
/// @version  1.0
/// @note
///////////////////////////////////////////////////////
#ifndef BLINKBTN_H
#define BLINKBTN_H
#include <QWidget>
#include <QBasicTimer>

class BlinkBtn : public QWidget
{
  Q_OBJECT
public:
  explicit BlinkBtn(const char* text,QWidget *parent = 0);
  ~BlinkBtn();
  void setTimeInterval(const int timeInterval);
  // 字体大小
  void setTextSize(int size);
  void setBold(bool isBold);
  void setOrder(bool isOrder);
public slots:
  // 字体内容
  void setText(const QString &text);
signals:
  void clicked();
  void doubleClicked();
  void normalShow();
protected:
  // 鼠标按下
  void mousePressEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  // 绘制文本
  void paintEvent(QPaintEvent *event);
  // 定时刷新
  void timerEvent(QTimerEvent *event);

private:
  bool m_bFullScreen;
  bool m_order;       // 设置文字闪烁的次序
  int m_nStep;        // 步调
  QPoint m_dragPosition;
  QBasicTimer m_timer;
  QString* m_pszText;
};

#endif // BLINKBTN_H
