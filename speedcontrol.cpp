#include "speedcontrol.h"
#include "buttonmx.h"
#include <QMouseEvent>
#include <QPushButton>
SpeedControl::SpeedControl(QWidget *parent):QLabel(parent)
{
  m_slowDownBtn = new ButtonMx("<<",this);
  m_recoverBtn = new ButtonMx("||",this);
  m_quickUpBtn = new ButtonMx(">>",this);

  m_slowDownBtn->setColorCustom(true);
  m_recoverBtn->setColorCustom(true);
  m_quickUpBtn->setColorCustom(true);

  int nWidth = this->width()/3-5;
  m_slowDownBtn->setFixedWidth(nWidth);
  m_recoverBtn->setFixedWidth(nWidth);
  m_quickUpBtn->setFixedWidth(nWidth);

  m_slowDownBtn->setColor("background-color: #DA70D6;");
  m_recoverBtn->setColor("background-color: #7B68EE;");
  m_quickUpBtn->setColor("background-color: #9400D3;");

  //this->setStyleSheet("background-color:#D2691E");
  //this->setStyleSheet("background-color: #421474;");
  connect(m_slowDownBtn,SIGNAL(chooseSIG()),this,SIGNAL(sigSlowDown()));
  connect(m_recoverBtn,SIGNAL(chooseSIG()),this,SIGNAL(sigRecover()));
  connect(m_quickUpBtn,SIGNAL(chooseSIG()),this,SIGNAL(sigQuickUp()));
}

void SpeedControl::resizeEvent(QResizeEvent *sizeEvt)
{
  Q_UNUSED(sizeEvt)
  layBtn();
}

void SpeedControl::layBtn()
{
  int nWidth = this->width()/3;
  m_slowDownBtn->setGeometry(0,0,nWidth,this->height());
  m_recoverBtn->setGeometry(nWidth,0,nWidth,this->height());
  m_quickUpBtn->setGeometry(2*nWidth,0,nWidth,this->height());
}

