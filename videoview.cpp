#include "videoview.h"

#include <QKeyEvent>
#include <QVBoxLayout>
#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QMediaPlayer>

VideoView::VideoView(QWidget *parent) :
  QLabel(parent)
{
  m_movice = nullptr;
  m_layout = nullptr;
  m_layout = new QVBoxLayout;
  setLayout(m_layout);
}

VideoView::~VideoView()
{
}

void VideoView::setHint(const QString &notify)
{
  this->setText(notify);
}

void VideoView::setVideo(QWidget *movies)
{
  if (!movies) return;
  m_movice = qobject_cast<VideoWidget*>(movies);
  m_layout->addWidget(m_movice);
  m_movice->setHidden(true);
}

void VideoView::waitingFor(int sec)
{
  QTimer::singleShot(sec, this, SLOT(onShowMovic()));
}

void VideoView::onShowMovic()
{
  this->clear();
  if (m_movice)
    {
      m_movice->setHidden(false);
    }
}

void VideoView::onHidden(bool hidden)
{
  if (m_movice) m_movice->setHidden(hidden);
  this->setHidden(hidden);
}


VideoWidget::VideoWidget(QWidget *parent):
  QVideoWidget(parent),m_bFull(false),m_player(nullptr)
{

}

void VideoWidget::setPlayer(QMediaPlayer *player)
{
  m_player = player;
}

void VideoWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
  Q_UNUSED(event)
  m_bFull = !m_bFull;
  setFullScreen(m_bFull);

  QWidget* father = qobject_cast<QWidget*> (parent());
  if(father) father->setHidden(m_bFull);

}

void VideoWidget::keyPressEvent(QKeyEvent *event)
{
  if(event->key() == Qt::Key_Escape)
    {
      setFullScreen(false);
    }
  QWidget::keyPressEvent(event);
}

void VideoWidget::wheelEvent(QWheelEvent *event)
{
  if ( m_player && event->orientation() == Qt::Vertical )
    {//垂直滚动
      int numDegrees = event->delta() / 8;
      int numSteps = numDegrees / 15;

      numSteps += m_player->volume();
      qDebug()<<numSteps;
      if(1 < numSteps) m_player->setVolume(numSteps);
    }
  event->accept();      //接收该事件
  return QWidget::wheelEvent(event);
}


void Lyric::openFile(const QString &fileName)
{

  m_lrc.clear();
  if(fileName.isEmpty())
    return;
  QString file_name = fileName;
  QString lrc_file_name = file_name.remove(file_name.right(3)) + "lrc";//把音频文件的后缀改成lrc后缀

  // 打开歌词文件
  QFile file(lrc_file_name);
  QString songName = QFileInfo(lrc_file_name).baseName();
  if (!file.open(QIODevice::ReadOnly)) {
      //m_lrc->setText(songName+ tr(" --- 未找到歌词文件！"));
      return ;
    }
  // 设置字符串编码
  //QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
  QString all_text = QString(file.readAll());
  file.close();
  // 将歌词按行分解为歌词列表
  QStringList lines = all_text.split("\n");

  //这个是时间标签的格式[00:05.54]
  //正则表达式d{2}表示匹配2个数字
  QRegExp rx("\\[\\d{2}:\\d{2}\\.\\d{2}\\]");
  foreach(QString oneline, lines) {
      QString temp = oneline;
      temp.replace(rx, "");//用空字符串替换正则表达式中所匹配的地方,这样就获得了歌词文本
      // 然后依次获取当前行中的所有时间标签，并分别与歌词文本存入QMap中
      //indexIn()为返回第一个匹配的位置，如果返回为-1，则表示没有匹配成功
      //正常情况下pos后面应该对应的是歌词文件
      int pos = rx.indexIn(oneline, 0);
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
          // 插入到lrc_map中
          m_lrc.insert(totalTime, temp);
          pos += rx.matchedLength();
          pos = rx.indexIn(oneline, pos);//匹配全部
        }
    }
  // 如果lrc_map为空
  if (m_lrc.isEmpty()) {
      return;
    }
}
