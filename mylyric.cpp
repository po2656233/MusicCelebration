///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file mylyric.cpp
/// @brief    歌词处理
/// @author   陈志华
/// @date     2017-4-18
/// @version  1.0
/// @note  将歌词文本改成UTF-8 BOM编码，以适应各平台
///////////////////////////////////////////////////////
#include "mylyric.h"
#include <QtGui/QPainter>
#include <QtCore/QTimer>
#include <QtGui/QMouseEvent>
#include <QtGui/QContextMenuEvent>
#include <QtWidgets/QMenu>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QApplication>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>
MyLyric::MyLyric(QWidget *parent) :
  QLabel(parent)
{
  setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
  // 设置背景透明
  setAttribute(Qt::WA_TranslucentBackground);
  setText(QStringLiteral("歌词信息"));
  setAlignment(Qt::AlignCenter);

  // 调整大小
  QDesktopWidget* desktop = QApplication::desktop();
//  QRect screenRect = desktop->screenGeometry();
  setMaximumSize(desktop->width()/2, 60);// 固定部件大小
  setMinimumSize(desktop->width()/2, 60);
  resize(desktop->width(), 60);
  move((desktop->width() - this->width()) / 2, desktop->height() * 3 / 4);

  // 歌词的线性渐变填充
  m_linearGradient.setStart(0, 10);
  m_linearGradient.setFinalStop(0, 40);
  m_linearGradient.setColorAt(0.1, QColor(14, 179, 255));
  m_linearGradient.setColorAt(0.5, QColor(114, 232, 255));
  m_linearGradient.setColorAt(0.9, QColor(14, 179, 255));
  // 遮罩的线性渐变填充
  m_maskLinearGradient.setStart(0, 10);
  m_maskLinearGradient.setFinalStop(0, 40);
  m_maskLinearGradient.setColorAt(0.1, QColor(222, 54, 4));
  m_maskLinearGradient.setColorAt(0.5, QColor(255, 72, 16));
  m_maskLinearGradient.setColorAt(0.9, QColor(222, 54, 4));
  // 设置字体
  m_font.setFamily("Times New Roman");
  m_font.setBold(true);
  m_font.setPointSize(30);
  // 设置定时器
  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
  m_lrcMaskWidth = 0;
  m_lrcMaskWidthInterval = 0;
}

MyLyric::~MyLyric()
{
clearAll();
}

bool MyLyric::loadLrc(const QString *fileName)
{
  if(!fileName) return false;
  QFileInfo fileInfo = QFileInfo(*fileName);
  //文件名
  QString fileNameLRC = fileInfo.absoluteFilePath();
  //文件后缀
  QString fileSuffix = fileInfo.suffix();
  QString lrc_file_name = fileNameLRC.remove(fileNameLRC.right(fileSuffix.length())) + "lrc";//把音频文件的后缀改成lrc后缀
  qDebug()<<"歌词文件:"<<lrc_file_name;
  // 打开歌词文件
  QFile file(lrc_file_name);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;


  // 清空之前的资源
  clearAll();

  // 提取全部内容
  QStringList all_text;
  QString songInfo;
  const char* key = NULL;
  QTextStream in(&file);
  while (!in.atEnd()) {
      songInfo = in.readLine();
      if (songInfo.startsWith("[ar:")) key = "ar";//演唱者 //曲名 专辑名 编者
      else if(songInfo.startsWith("[ti:")) key = "ti";
      else if(songInfo.startsWith("[al:")) key = "al";
      else if(songInfo.startsWith("[by:")) key = "by";
      else if(songInfo.startsWith("[offset:时间补偿值")) key = "offset";
      else
        {
          key = NULL;
          all_text<<songInfo;
          continue;
        }
      if(NULL != key)
        {
          QString* strVale = new QString(songInfo.mid(4,songInfo.length()-5));
          m_songInfo.insert(key,strVale);
        }
    }
  file.close();

  // 按歌词格式保存信息
  QRegExp rx("\\[\\d{2}:\\d{2}\\.\\d{2}\\]");
  foreach (QString item, all_text) {
      QString temp = item;
      temp.replace(rx, "");//用空字符串替换正则表达式中所匹配的地方,这样就获得了歌词文本
      // 然后依次获取当前行中的所有时间标签，并分别与歌词文本存入QMap中
      //indexIn()为返回第一个匹配的位置，如果返回为-1，则表示没有匹配成功
      //正常情况下pos后面应该对应的是歌词文件
      int pos = rx.indexIn(item, 0);
      while (pos != -1) { //表示匹配成功
          QString cap = rx.cap(0);//返回第0个表达式匹配的内容
          // 将时间标签转换为时间数值，以毫秒为单位
          QRegExp regexp;
          regexp.setPattern("\\d{2}(?=:)");
          regexp.indexIn(cap);
          int minute = regexp.cap(0).toInt();
          regexp.setPattern("\\d{2}(?=\\.)");
          regexp.indexIn(cap);
          int second = regexp.cap(0).toInt();
          regexp.setPattern("\\d{2}(?=\\])");
          regexp.indexIn(cap);
          int millisecond = regexp.cap(0).toInt();
          qint64 totalTime = minute * 60000 + second * 1000 + millisecond * 10;
          // m_lrc
          QString* value = new QString(temp);
          m_lrc.insert(totalTime, value);
          pos += rx.matchedLength();
          pos = rx.indexIn(item, pos);//匹配全部
        }
    }
  show();
  return !m_lrc.isEmpty();
}


void MyLyric::paintEvent(QPaintEvent *)
{
  QPainter painter(this);
  painter.setFont(m_font);
  // 先绘制底层文字，作为阴影，这样会使显示效果更加清晰，且更有质感
  painter.setPen(QColor(0, 0, 0, 200));
  painter.drawText(1, 1, 800, 60, Qt::AlignLeft, text());
  // 再在上面绘制渐变文字
  painter.setPen(QPen(m_linearGradient, 0));
  painter.drawText(0, 0, 800, 60, Qt::AlignLeft, text());


  // 设置歌词遮罩
  painter.setPen(QPen(m_maskLinearGradient, 0));
  painter.drawText(0, 0, m_lrcMaskWidth, 60, Qt::AlignLeft, text());
}


// 开启遮罩，需要指定当前歌词开始与结束之间的时间间隔
void MyLyric::startLrcMask(qint64 intervalTime)
{
  // 这里设置每隔30毫秒更新一次遮罩的宽度，因为如果更新太频繁
  // 会增加CPU占用率，而如果时间间隔太大，则动画效果就不流畅了
  qreal count = intervalTime / 30;
  // 获取遮罩每次需要增加的宽度，这里的800是部件的固定宽度
  m_lrcMaskWidthInterval = 800 / count;
  m_lrcMaskWidth = 0;
  m_timer->start(30);
}


// 停止遮罩
void MyLyric::stopLrcMask()
{
  m_timer->stop();
  m_lrcMaskWidth = 0;
  update();
}

void MyLyric::clearAll()
{
  QMap<quint64,QString*>::const_iterator iter = m_lrc.constBegin();
  while (iter != m_lrc.constEnd()) {
      iter.value()->clear();
      delete iter.value();
      ++iter;
    }
  this->clear();
  m_lrc.clear();
  clearInfo();
}

void MyLyric::updateTime(qint64 time)
{
  qint64 prev = 0, next = 0;
  foreach(qint64 value, m_lrc.keys())
    {
      if (time >= value)
        prev = value;
      else{
          next = value;
          break;
        }
    }
  QString* song = m_lrc.value(prev);
  if(!song || song->isEmpty())return;
  QString currentLrc = *song;
  if (currentLrc.length() < 2)
    currentLrc = windowTitle();
  if (currentLrc != text())
    {
      setText(currentLrc);
      qint64 intervalTime = next - prev;
      startLrcMask(intervalTime);
    }
}


// 两个鼠标事件处理函数实现了部件的拖动
void MyLyric::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    m_offset = event->globalPos() - frameGeometry().topLeft();
  QLabel::mousePressEvent(event);
}
void MyLyric::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton) {
      setCursor(Qt::PointingHandCursor);
      move(event->globalPos() - m_offset);
    }
  QLabel::mouseMoveEvent(event);
}


// 实现右键菜单来隐藏部件
void MyLyric::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu menu;
  menu.addAction(QStringLiteral("隐藏"), this, SLOT(hide()));
  menu.exec(event->globalPos());
}


// 定时器溢出时增加遮罩的宽度，并更新显示
void MyLyric::timeout()
{
  m_lrcMaskWidth += m_lrcMaskWidthInterval;
  update();
}

void MyLyric::clearInfo()
{
  QMap<const char*,QString*>::const_iterator iter = m_songInfo.constBegin();
  while (iter != m_songInfo.constEnd()) {
      iter.value()->clear();
      delete iter.value();
      ++iter;
    }
  m_songInfo.clear();
}
