///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file     musicshow.h
/// @brief    视频界面展示
/// @author   陈志华
/// @date     2017-4-18
/// @version  1.0
/// @note     实现视频的整体界面展示：
/// 涵盖：
/// 1、观影前提示
/// 2、延时播放
///////////////////////////////////////////////////////
#ifndef VIDEOVIEW_H
#define VIDEOVIEW_H
#include <QVideoWidget>
#include <QLabel>
#include <QMap>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QMediaPlayer;
QT_END_NAMESPACE

class VideoView : public QLabel
{
  Q_OBJECT
public:
  explicit VideoView(QWidget *parent = 0);
  ~VideoView();
  void setHint(const QString& notify);//观影前提示
  void setVideo(QWidget* movies);//放置影片
  void waitingFor(int sec=100);//延时

public slots:
  void onShowMovic();
  void onHidden(bool hidden);

private:
  QWidget       *m_movice;
  QVBoxLayout   *m_layout;
};
class VideoWidget:public QVideoWidget
{
  Q_OBJECT
public:
  explicit VideoWidget(QWidget *parent = 0);
  void setPlayer(QMediaPlayer* player);
protected:
  void mouseDoubleClickEvent(QMouseEvent *event);
  void keyPressEvent(QKeyEvent *event);
  void wheelEvent(QWheelEvent *event);
private:
  bool m_bFull:1;
  QMediaPlayer* m_player;
};

class Lyric: public QWidget
{
  Q_OBJECT
public:
  explicit Lyric(QWidget *parent = 0);
  void openFile(const QString& fileName);
private:
  QMap<quint64,QString> m_lrc;

};
#endif // VIDEOVIEW_H
