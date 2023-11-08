///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file
/// @brief
/// @author
/// @date
/// @version  1.0
/// @note
///////////////////////////////////////////////////////
#include "blinkbtn.h"
#include <QFont>
#include <QTimer>
#include <QPainter>

#include <QTimerEvent>
#include <QMouseEvent>

BlinkBtn::BlinkBtn(const char* text,QWidget *parent) :
  QWidget(parent),m_bFullScreen(false),m_order(false),m_pszText(0)
{
  m_pszText = new QString(text);
  setAutoFillBackground(true);
  this->setStyleSheet("color:rgb(30,144,255);");
}

BlinkBtn::~BlinkBtn()
{
  if(m_pszText) delete m_pszText;
  m_timer.stop();
}

void BlinkBtn::setTimeInterval(const int timeInterval)
{
  m_timer.start(timeInterval, this);
}

void BlinkBtn::setTextSize(int size)
{
  QFont newFont = font();
  newFont.setPointSize(newFont.pointSize() + size);
  setFont(newFont);
}

void BlinkBtn::setBold(bool isBold)
{
  QFont newFont = font();
  newFont.setBold(isBold);
  setFont(newFont);
}

void BlinkBtn::setOrder(bool isOrder)
{
  m_order = isOrder;
}

void BlinkBtn::setText(const QString &text)
{
  m_pszText->clear();
  m_pszText->append(text);
}

void BlinkBtn::mousePressEvent(QMouseEvent *event)
{
  //m_dragPosition = event->globalPos() - pos();
  event->accept();
  emit clicked();
  QWidget::mousePressEvent(event);
}

void BlinkBtn::mouseDoubleClickEvent(QMouseEvent *event)
{
  m_bFullScreen = !m_bFullScreen;
  if (m_bFullScreen)
    emit doubleClicked();
  else
    emit normalShow();
  QWidget::mouseDoubleClickEvent(event);
}

void BlinkBtn::mouseMoveEvent(QMouseEvent *event)
{
  //move(event->globalPos() - m_dragPosition);
  event->accept();
  QWidget::mouseMoveEvent(event);
}

void BlinkBtn::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  // 计算文本绘制的起始坐标
  QFontMetrics metrics(font());
  int nIndex = 0;
  int x = (width() - metrics.width(*m_pszText)) / 2;//文字居中
  int y = (height() + metrics.ascent() - metrics.descent()) / 2;//文字居中

  QColor color;
  QPainter painter(this);
  if(m_order)
    {
      for (int i = 0; i < m_pszText->size(); ++i)
        {
          // 设置色调（H）、饱和度（S）、亮度（V）
          nIndex = (m_nStep + i) % 16;
          color.setHsv((15 - nIndex) * 10, 255, 191);
          painter.setPen(color);

          // 单个字符绘制
          painter.drawText(x, y, m_pszText->at(i));

          // 计算下一个字符的x坐标起始点
          x += metrics.width(m_pszText->at(i));
        }
    }
  else
    {
      int i = m_pszText->size();
      int nPos = 0;

      while(i-- != 0)
        {
          nIndex = (m_nStep + i) % 16;
          color.setHsv((15 - nIndex) * 10, 210, 150);
          painter.setPen(color);

          // 单个字符绘制
          painter.drawText(x, y, m_pszText->at(nPos));

          // 计算下一个字符的x坐标起始点
          x += metrics.width(m_pszText->at(nPos));
          nPos++;
        }
    }
}

void BlinkBtn::timerEvent(QTimerEvent *event)
{
  if (event->timerId() == m_timer.timerId())
    {
      ++m_nStep;
      update();
    }
  else
    {
      QWidget::timerEvent(event);
    }
}
