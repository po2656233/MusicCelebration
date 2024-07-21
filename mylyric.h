///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file mylyric.h
/// @brief    歌词处理
/// @author   陈志华
/// @date     2017-4-18
/// @version  1.0
/// @note     歌词处理 (需将歌词文本改成UTF-8 BOM编码，以适应各平台)
/// 1、加载歌词文件
/// 2、获取歌曲信息
/// 3、遮罩展示
///////////////////////////////////////////////////////
#ifndef MYLYRIC_H
#define MYLYRIC_H

#include <QLabel>
#include <QMap>
class MyLyric : public QLabel
{
  Q_OBJECT
public:
  explicit MyLyric(QWidget *parent = 0);
  ~MyLyric();
  // 加载歌词文件
  bool loadLrc(const QString* fileName);
  // 获取歌曲信息
  QString* getSongInfo(const char* key);
  // 开始遮罩
  void startLrcMask(qint64 intervalTime);
  // 关闭遮罩
  void stopLrcMask();
  // 清空资源
  void clearAll();

signals:
  void hideSong();

public slots:
  void updateTime(qint64 time);
protected:
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *ev);
  void mouseMoveEvent(QMouseEvent *ev);
  void contextMenuEvent(QContextMenuEvent * event);
private slots:
  void timeout();

private:
  void clearInfo();

private:
  QTimer*   m_timer;
  qreal     m_lrcMaskWidth;
  qreal     m_lrcMaskWidthInterval;
  QLinearGradient m_linearGradient;
  QLinearGradient m_maskLinearGradient;
  QFont m_font;
  QPoint m_offset;
  QMap<const char*, QString*> m_songInfo;
  QMap<quint64,QString*> m_lrc;
};

#endif // MYLYRIC_H
