///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file
/// @brief
/// @author
/// @date
/// @version  1.0
/// @note
///////////////////////////////////////////////////////
#include "musicshow.h"
#include "widgetObjName.h"
#include "videoview.h"
#include "dialogmx.h"
#include "buttonmx.h"
#include "blinkbtn.h"
#include "speedcontrol.h"
#include "webview.h"

#include "mylyric.h"

//#include <windows.h>
//#include <windowsx.h>
//控件
#include <QLabel>
#include <QPushButton>
#include <QDial>
#include <QLCDNumber>
#include <QFileDialog>
#include <QMenu>
#include <QLineEdit>
#include <QTextBrowser>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QMessageBox>

//数据
#include <QApplication>
#include <QDir>
#include <QUrl>
#include <QFile>
#include <QTime>
#include <QTimer>
#include <QEvent>
#include <QUrl>
#include <QPixmap>

#include <QListView>
#include <QStringListModel>
#include <QPalette>
#include <QKeyEvent>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QThread>

#include <QDebug>

#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QtMultimediaWidgets/QVideoWidget>

static qreal g_rate = 1.0;
static bool isNoBorder = false;
static bool isMute = false;
static bool isDir = true;//是否目录
static bool isLeftPressDown = false;//鼠标左键按下
static bool isSigleton = false;
bool isLoadNow = false; //是否正在加载中
#define PADDING 2 //边距

MusicShow::MusicShow(QWidget *parent) :
    QWidget(parent),m_isTop(false),
    m_isLrc(false),m_isShowLrc(true),m_isVideo(false),
    m_duration(0),m_dragPosition(QPoint(0,0))
{

    // 保存歌曲的目录
    m_songsDir = QDir::currentPath()+"\\songs";
    QDir dir(m_songsDir);
    if(!dir.exists())dir.mkdir(m_songsDir);

    m_playInfo = new QLabel(tr("播放信息")); //播放信息
    m_title = new BlinkBtn("音乐分享"); //标题
    m_speedControl = new SpeedControl();//进度标题

    m_playMode = new QPushButton("PLAY MODE"); //播放模式
    m_loading = new QPushButton("START");  //加载
    m_paused = new QPushButton("PAUSED");   //暂停
    m_play = new QPushButton("PLAY");     //播放
    m_stop = new QPushButton("STOP");     //停止

    m_model = new QStringListModel();
    m_listView = new QListView();     // 播放列表
    m_fileList = new QMediaPlaylist();  // 播放资源列表
    m_player = new QMediaPlayer();    // 播放控制器
    m_view = new VideoWidget();       // 视频播放
    m_video = new VideoView();        // 视频展示
    m_songLrc = new MyLyric(this);       // 歌词显示

    m_lound = new QDial();            // 声音
    m_timeUp = new QLCDNumber();      // 时间
    m_horizontalSlider = new QSlider(Qt::Horizontal);//进度条

    m_networdShow = new WebView(this);
    m_layout = new QGridLayout();//布局
    m_popMenu = new QMenu();//右键弹出的菜单
    m_actBorders = new QAction(this);//右键无边框显示
    m_actQuit = new QAction(this);//右键退出
    m_actTop = new QAction(this);//置顶
    m_actHide = new QAction(this);//隐藏
    m_actMute = new QAction(this);//静音
    m_actLry = new QAction(this);//显示歌词
    m_actSigleton = new QAction(this);//单窗体


    this->setMouseTracking(true); // 希望获取到鼠标移动时，光标靠近的位置。
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->resize(800,500);

    // 布局
    this->setLayout(m_layout);

    //m_layout->addWidget(m_songLrc,1,0,1,1);//歌词
    m_layout->addWidget(m_loading,5,0,1,1);
    m_layout->addWidget(m_speedControl,6,0,1,1);
    m_layout->addWidget(m_title,0,1,1,4);
    m_layout->addWidget(m_video,0,0,5,6);
    m_layout->addWidget(m_listView,1,1,4,4);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(m_play);
    hLayout->addWidget(m_paused);
    hLayout->addWidget(m_stop);
    m_layout->addLayout(hLayout,5,1,1,4);

    m_layout->addWidget(m_horizontalSlider,6,1,1,4);
    m_layout->addWidget(m_playInfo,0,5,1,1,Qt::AlignHCenter);
    m_layout->addWidget(m_lound,2,5,1,1);
    m_layout->addWidget(m_playMode,5,5,1,1);
    m_layout->addWidget(m_timeUp,6,5,1,1);

    // 设置比例：
    //  m_layout->addWidget(m_networdShow,0,1,5,4);
    //  m_networdShow->setHidden(true);

    // 界面数据初始化
    m_play->setObjectName(objNames::BUTTON_PLAY);
    m_title->setObjectName(objNames::WIDGET_TITEL);
    m_title->setTextSize(20);
    m_title->setBold(true);
    m_title->setTimeInterval(100);
    m_popMenu->heightForWidth(80);
    m_timeUp->display("00:00");
    m_actTop->setText(tr("拼醉花前"));
    m_actMute->setText(tr("颜羞羞言"));
    m_actHide->setText(tr("思念于心"));
    m_actSigleton->setText(tr("一幕了然"));
    m_actBorders->setText(tr("万寿无疆"));
    m_actLry->setText(tr("英花逐流"));
    m_actQuit->setText(tr("闭月羞花"));

    // 快捷键
    m_play->setShortcut(QKeySequence(Qt::Key_Return));
    m_paused->setShortcut(QKeySequence(Qt::Key_Space));
    m_stop->setShortcut(QKeySequence(Qt::Key_Escape));

    // 设置字体颜色

    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::blue);
    m_playInfo->setPalette(pa);
    m_playInfo->setFont(QFont("FangSong", 18));

    //QPalette pal1 = m_loading->palette();
    pa.setColor(QPalette::ButtonText,QColor(220,20,60));
    m_loading->setPalette(pa);
    pa.setColor(QPalette::ButtonText,QColor(125,0,125));
    m_playMode->setPalette(pa);

    QFont font = m_loading->font();
    font.setBold(true);
    m_loading->setFont(font);
    m_playMode->setFont(font);

    // 播放器
    m_player->setMuted(false);//是否静音
    m_player->setVideoOutput(m_view);
    m_player->setPlaylist(m_fileList);
    m_video->setHidden(true);
    m_video->setVideo(m_view);
    //    m_view->setPlayer(m_player);
    m_view->setAspectRatioMode(Qt::IgnoreAspectRatio);
    m_lound->setValue(50);
    m_lound->setNotchesVisible(true);
    m_listView->setWordWrap(true);
    m_listView->setModel(m_model);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    // 关联信号和槽
    //加载文件
    connect(m_loading,SIGNAL(clicked()),this,SLOT(onLoading()));
    //点击播放
    connect(m_play,SIGNAL(clicked(bool)),m_player,SLOT(play()));
    //点击暂停
    connect(m_paused,SIGNAL(clicked(bool)),m_player,SLOT(pause()));
    //点击停止
    connect(m_stop,SIGNAL(clicked(bool)),m_player,SLOT(stop()));

    //文件列表
    connect(m_listView,SIGNAL(pressed(QModelIndex)),this,SLOT(onSelectitem_singal(QModelIndex)));
    connect(m_listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onSelectitem(QModelIndex)));
    connect(m_player, SIGNAL(durationChanged(qint64)), this, SLOT(onDuration(qint64)));
    connect(m_player, SIGNAL(positionChanged(qint64)), this, SLOT(onPlayTimer(qint64)));

    //进度条
    connect(m_horizontalSlider, SIGNAL(sliderMoved(int)), this, SLOT(onSeek(int)));
    //声音
    connect(m_lound,SIGNAL(valueChanged(int)),this, SLOT(on_lound_sliderMoved(int)));

    //播放模式
    connect(m_playMode,SIGNAL(clicked(bool)),this,SLOT(on_playModel_clicked()));
    //媒体状态
    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(onStatus(QMediaPlayer::State)));
    connect(m_player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(onMediastatus(QMediaPlayer::MediaStatus)) ) ;
    connect(m_player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(on_err(QMediaPlayer::Error)));
    connect(m_fileList, SIGNAL(currentIndexChanged(int)),this, SLOT(onSingTheSong(int)));

    //右键弹框
    connect(m_actHide, &QAction::triggered, this, &MusicShow::close);
    connect(m_actMute, SIGNAL(triggered(bool)), this, SLOT(onMuted(bool)));
    connect(m_actBorders, &QAction::triggered, this, &MusicShow::on_noBoardStyle);
    connect(m_actTop, &QAction::triggered, this, &MusicShow::on_topWindow);
    connect(m_actSigleton, &QAction::triggered, this, &MusicShow::onSigletonShow);
    connect(m_actLry, &QAction::triggered, this, &MusicShow::onSongShow);
    connect(m_actQuit,SIGNAL(triggered()),qApp,SLOT(quit()));
    //播放速度
    connect(m_player,SIGNAL(playbackRateChanged(qreal)),m_player,SLOT(setPlaybackRate(qreal)));
    connect(m_speedControl,SIGNAL(sigSlowDown()),this,SLOT(onSlowDown()));
    connect(m_speedControl,SIGNAL(sigRecover()),this,SLOT(onRecover()));
    connect(m_speedControl,SIGNAL(sigQuickUp()),this,SLOT(onQuickUp()));
    //窗口模式切换
    connect(m_title,SIGNAL(doubleClicked()),this,SLOT(showFullScreen()));
    connect(m_title,SIGNAL(normalShow()),this,SLOT(showNormal()));
}

MusicShow::~MusicShow()
{
    if(m_model)
    {
        delete m_model;
        m_model = nullptr;
    }
    if (m_player)
    {
        delete m_player;
        m_player = nullptr;
    }
    if(m_layout)
    {
        delete m_layout;
        m_layout = nullptr;
    }
    if(m_horizontalSlider)
    {
        delete m_horizontalSlider;
        m_horizontalSlider = nullptr;
    }
    if(m_timeUp)
    {
        delete m_timeUp;
        m_timeUp = nullptr;
    }
    if(m_lound)
    {
        delete m_lound;
        m_lound = nullptr;
    }
    if(m_view)
    {
        delete m_view;
        m_view = nullptr;
    }
    if(m_songLrc)
    {
        delete m_songLrc;
        m_songLrc = nullptr;
    }
    if (m_fileList)
    {
        delete m_fileList;
        m_fileList = nullptr;
    }
    if(m_listView)
    {
        delete m_listView;
        m_listView = nullptr;
    }
    if(m_stop)
    {
        delete m_stop;
        m_stop = nullptr;
    }
    if(m_play)
    {
        delete m_play;
        m_play = nullptr;
    }
    if(m_paused)
    {
        delete m_paused;
        m_paused = nullptr;
    }
    if(m_loading)
    {
        delete m_loading;
        m_loading = nullptr;
    }
    if(m_playMode)
    {
        delete m_playMode;
        m_playMode = nullptr;
    }
    if(m_title)
    {
        delete m_title;
        m_title = nullptr;
    }
    if(m_playInfo)
    {
        delete m_playInfo;
        m_playInfo = nullptr;
    }
    /*
  if (m_networdShow)
    {
      delete m_networdShow;
      m_networdShow = nullptr;
    }*/
    if (m_actQuit)
    {
        delete m_actQuit;
        m_actQuit = nullptr;
    }
    if (m_actBorders)
    {
        delete m_actBorders;
        m_actBorders = nullptr;
    }
    if(m_actTop)
    {
        delete m_actTop;
        m_actTop = nullptr;
    }
    if (m_popMenu)
    {
        delete m_popMenu;
        m_popMenu = nullptr;
    }
    if(m_actHide)
    {
        delete m_actHide;
        m_actHide = nullptr;
    }
    if(m_actMute)
    {
        delete m_actMute;
        m_actMute = nullptr;
    }
}


QUrl MusicShow::playing()
{
    if(!m_playing.isValid())return QUrl();
    return m_playing;
}

// 随机播放
void MusicShow::randomPlay()
{
    // 不重复随机数
    static QTime static_t = QTime::currentTime();
    QTime time = QTime::currentTime();
    int sec = static_t.msecsTo(time);
    qsrand(sec);
    int math = qrand()% m_songList.size();

    QString fileName = m_songList.at(math).fileName();
    //playSong(fileName);
    for (int i = 0; i < m_model->rowCount(); i++)
    {
        if (fileName == m_model->index(i).data().toString())
        {
            m_listView->setCurrentIndex(m_model->index(i));
        }
    }
    m_listView->setSelectionRectVisible(true);
}

void MusicShow::playModel(int choose)
{
    QMediaPlaylist::PlaybackMode model = static_cast<QMediaPlaylist::PlaybackMode>(choose);
    m_fileList->setPlaybackMode(model);
}

void MusicShow::sigletonShow(bool isShow)
{
    m_loading->setHidden(isShow);
    //m_lound->setHidden(isShow);
    m_paused->setHidden(isShow);
    m_play->setHidden(isShow);
    m_stop->setHidden(isShow);
    m_speedControl->setHidden(isShow);
    m_playMode->setHidden(isShow);
    m_horizontalSlider->setHidden(isShow);
    m_timeUp->setHidden(isShow);
}

bool MusicShow::checkSong(const QString &songName)
{

    if(songName.isNull()||songName.isEmpty())return false;
    synchronyLrc(songName);

    return( songName.contains(".mp3") || songName.contains(".ape")|| songName.contains(".flac"));
}

// 获取所有文件列表
QStringList MusicShow::getAllFiles(const QString &strDir)
{
    QDir dir(strDir);
    if(!dir.exists())
        return QStringList();
    dir.setFilter(QDir::Dirs|QDir::Files);
    dir.setSorting(QDir::DirsFirst|QDir::Time|QDir::Reversed);
    QStringList fileList;
    fileList<<"*.mp4"<<"*.mp3"<<"*.mkv"<<"*.mov"<<"*.wma"<<"*.wav"\
           <<"*.mpeg"<<"*.mpeg1"<<"*.mpeg2"<<"*.mpeg4"\
          <<"*.rm"<<"*.rmvb"\
         <<"*.ape"<<"*.aac"<<"*.avi"\
        <<"*.ogg"<<"*.dat"\
       <<"*.flv"<<"*.gif"<<"*.asf"<<"*.flac";
    dir.setNameFilters(fileList);

    // 目录
    m_fileList->clear();
    m_songList.clear();
    m_songList = dir.entryInfoList(fileList, QDir::Files|QDir::AllEntries);

    // 读取文件信息列表
    fileList.clear();
    foreach (QFileInfo info, m_songList)
    {
        fileList<<info.fileName();
        m_fileList->addMedia( QUrl(info.absoluteFilePath()) );
    }
    if(!fileList.isEmpty())m_model->setStringList(fileList);
    return fileList;
}

void MusicShow::synchronyLrc(const QString &fileName)
{
    qDebug()<<"当前曲目:"<<fileName;
    m_songLrc->stopLrcMask();
    m_songLrc->clear();
    m_isLrc = m_songLrc->loadLrc(&fileName);
    if (!m_isLrc) m_songLrc->setText(QStringLiteral("歌词信息"));
    if(m_isShowLrc) m_songLrc->show();
}
// 选择列表当中的曲目
void MusicShow::onSelectitem(const QModelIndex &index)
{
    if (!index.isValid())return;
    for (int i = 0; i<m_fileList->mediaCount(); i++)
    {
        if (m_fileList->media(i).canonicalUrl().fileName() == index.data().toString())
        {
            m_playing = m_fileList->media(i).canonicalUrl();
            m_fileList->setCurrentIndex(i);
            m_songLrc->stopLrcMask();
            m_songLrc->clear();
            listTurnVedio( !checkSong(m_fileList->currentMedia().canonicalUrl().toString()) );
            break;
        }
    }
    if (m_player->isAudioAvailable() || m_player->isVideoAvailable() || m_player->isMetaDataAvailable() || m_player->isAvailable())
        m_player->play();
}

void MusicShow::onSelectitem_singal(const QModelIndex &index)
{
    m_listView->setToolTip(index.data().toString());
    changeMouseIcon('D');
}

// 播放状态
void MusicShow::onStatus(QMediaPlayer::State status)
{
    onRecover(); //标准速率
    switch (status)
    {
    case QMediaPlayer::PlayingState:
    {
        m_isPlayer = true;
        sliderStyle(true);
        // listTurnVedio( !checkSong(m_playing.fileName()) );
        m_listView->setToolTip(m_playing.fileName());
    }
        break;
    case QMediaPlayer::PausedState:
    {
        sliderStyle(false);
        m_isPlayer = false;
    }
        break;
    case QMediaPlayer::StoppedState:
    {
        m_isPlayer = false;
        sliderStyle(false);
        listTurnVedio(false);
    }
        break;
    default:
        break;
    }


}
/*
// 键盘事件（调整播放状态）
void MusicShow::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
    case Qt::Key_Enter:
      {
        if(isPlay)
          {
            isPlay = false;
            emit m_player->stateChanged(QMediaPlayer::PausedState);
          }
        else
          {
            isPlay = true;
            emit m_player->stateChanged(QMediaPlayer::PlayingState);
          }
        break;
      }
    case Qt::Key_Space:
      {
        if(isPlay)
          {
            isPlay = false;
            emit m_player->stateChanged(QMediaPlayer::PausedState);
          }
        else
          {
            isPlay = true;
            emit m_player->stateChanged(QMediaPlayer::PlayingState);
          }
        break;
      }
    case Qt::Key_Escape:
      {
        this->showNormal();
        break;
      }
    default:
      break;
    }
  event->ignore();
  QWidget::keyPressEvent(event);
}
*/
// 双击全屏
void MusicShow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        windowState()!=Qt::WindowFullScreen?setWindowState(Qt::WindowFullScreen):setWindowState(Qt::WindowNoState);
    }
    event->ignore();
    QWidget::mouseDoubleClickEvent(event);
}

void MusicShow::mousePressEvent(QMouseEvent *event)
{
    if (Qt::LeftButton == event->button() || Qt::RightButton == event->button())
    {
        isLeftPressDown = true;
        if(m_direct != NONE)
        {
            this->mouseGrabber();
        }
        else
        {
            m_dragPosition = event->globalPos() - pos();
        }
    }
    event->ignore();
}

void MusicShow::mouseMoveEvent(QMouseEvent *event)
{
    //changeMouseIcon('C');
    QPoint gloPoint = event->globalPos();
    QRect rect = this->rect();
    QPoint tl = mapToGlobal(rect.topLeft());
    QPoint rb = mapToGlobal(rect.bottomRight());

    if(!isLeftPressDown)
    {
        this->region(gloPoint);
    }
    else
    {
        if(m_direct != NONE)
        {
            if( isLeftPressDown && (m_isPlayer || m_isVideo) ) m_video->setHidden(true);
            QRect rMove(tl, rb);
            switch(m_direct)
            {
            case LEFT:
                if(rb.x() - gloPoint.x() <= this->minimumWidth())
                    rMove.setX(tl.x());
                else
                    rMove.setX(gloPoint.x());
                break;
            case RIGHT:
                rMove.setWidth(gloPoint.x() - tl.x());
                break;
            case UP:
                if(rb.y() - gloPoint.y() <= this->minimumHeight())
                    rMove.setY(tl.y());
                else
                    rMove.setY(gloPoint.y());
                break;
            case DOWN:
                rMove.setHeight(gloPoint.y() - tl.y());
                break;
            case LEFTTOP:
                if(rb.x() - gloPoint.x() <= this->minimumWidth())
                    rMove.setX(tl.x());
                else
                    rMove.setX(gloPoint.x());
                if(rb.y() - gloPoint.y() <= this->minimumHeight())
                    rMove.setY(tl.y());
                else
                    rMove.setY(gloPoint.y());
                break;
            case RIGHTTOP:
                rMove.setWidth(gloPoint.x() - tl.x());
                rMove.setY(gloPoint.y());
                break;
            case LEFTBOTTOM:
                rMove.setX(gloPoint.x());
                rMove.setHeight(gloPoint.y() - tl.y());
                break;
            case RIGHTBOTTOM:
                rMove.setWidth(gloPoint.x() - tl.x());
                rMove.setHeight(gloPoint.y() - tl.y());
                break;
            default:
                break;
            }
            this->setGeometry(rMove);
        }
        else
        {
            if (m_dragPosition!=QPoint(0,0)) move(gloPoint - m_dragPosition);
            event->accept();
        }
    }
    event->ignore();
    QWidget::mouseMoveEvent(event);
}

void MusicShow::mouseReleaseEvent(QMouseEvent *event)
{
    isLeftPressDown = false;
    m_dragPosition = QPoint(0,0);
    QApplication::restoreOverrideCursor();

    if (m_isPlayer && m_isVideo)
    {
        listTurnVedio(m_isVideo);
    }

    event->ignore();
    QWidget::mouseReleaseEvent(event);
}

void MusicShow::contextMenuEvent(QContextMenuEvent *event)
{
    m_popMenu->clear();
    m_popMenu->addAction(m_actTop);
    //m_popMenu->addAction(m_actBorders);//有无边框
    m_popMenu->addAction(m_actHide);
    m_popMenu->addAction(m_actMute);
    m_popMenu->addAction(m_actLry);
    m_popMenu->addAction(m_actSigleton);
    m_popMenu->addAction(m_actQuit);



    //菜单出现的位置为当前鼠标的位置
    m_popMenu->exec(QCursor::pos());
    event->accept();
}

void MusicShow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->setGeometry(this->geometry());
    hide();
    emit signalHide();

}

void MusicShow::wheelEvent(QWheelEvent *event)
{
    if (event->orientation() == Qt::Vertical && m_isPlayer)
    {//垂直滚动
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;
        numSteps += m_lound->value();
        m_lound->setValue(numSteps);
    }
    event->accept();      //接收该事件
    return QWidget::wheelEvent(event);
}


void MusicShow::onLoading()
{
    DialogMx *loadDlg = new DialogMx(this);
    loadDlg->setGeometry(this->pos().x()+10,this->pos().y()+this->height()-170,100,100);
    if(m_isTop)
        loadDlg->setWindowFlags(Qt::WindowStaysOnTopHint|Qt::Dialog|Qt::FramelessWindowHint);
    else
        loadDlg->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);

    loadDlg->setAttribute(Qt::WA_DeleteOnClose,true);
    loadDlg->setModal(true);
    loadDlg->showNormal();
    //m_layout->addWidget(loadDlg,4,5,1,1,Qt::AlignRight);

    ButtonMx* btnLocal = new ButtonMx("本地导入",loadDlg);
    ButtonMx* btnWeb = new ButtonMx("加载网络资源",loadDlg);
    // 字体样式
    QFont loadFt = QFont("STLiti", 10, QFont::DemiBold);
    QPalette loadPa;
    loadPa.setColor(QPalette::WindowText,QColor(100,0,138));

    btnLocal->setPalette(loadPa);
    btnLocal->setFont(loadFt);

    btnWeb->setPalette(loadPa);
    btnWeb->setFont(loadFt);

    QVBoxLayout* fLayout = new QVBoxLayout;
    fLayout->addWidget(btnLocal);
    fLayout->addWidget(btnWeb);
    loadDlg->setLayout(fLayout);

    connect(btnLocal, &ButtonMx::chooseSIG, this, [=](){
        delete loadDlg;
        on_loading_dir();
    });

    connect(btnWeb,&ButtonMx::chooseSIG, this, [=](){
        delete loadDlg;
        on_loading_web();
    });


    //  isDir?on_loading_dir():on_loading_single();
    //  isDir = !isDir;

}

void MusicShow::on_loading_dir()
{
    isDir = true;
    m_player->stop();
    //  m_networdShow->stop();
    //  m_networdShow->setHidden(true);


    QString fileName = QFileDialog::getExistingDirectory(this,"Music",m_songsDir,QFileDialog::ReadOnly);
    if (fileName.isNull() || fileName.isEmpty())return;
    getAllFiles(fileName);
}


void MusicShow::on_loading_web()
{
    m_loading->setHidden(true);
    QLineEdit* inputWeb = new QLineEdit();
    inputWeb->setFixedWidth(m_loading->width());
    inputWeb->setPlaceholderText("请录入歌名或网址");
    m_layout->addWidget(inputWeb,2,0,1,1);
    isLoadNow = false;
    connect(inputWeb,&QLineEdit::returnPressed,this,[=](){
        // 禁止频繁 回车
        if(isLoadNow)return;
        isLoadNow = !isLoadNow;
        // 解析地址
        QString webAddr = inputWeb->text();
        if (webAddr.isEmpty()){
            m_loading->setHidden(false);
            if (inputWeb)inputWeb->deleteLater();
            return;
        }
        if (webAddr.left(4) != "http")webAddr = "http://"+webAddr;

        // 获取歌曲
        QString singName = m_networdShow->setUrl(webAddr,m_songsDir);

        if(singName.isEmpty())return;

        // 停止当前曲目
        m_fileList->clear();
        //        m_player->stop();

        // 添加入曲目列表
        QFileInfo info(singName);
        m_fileList->addMedia( QUrl(info.absoluteFilePath()));
        m_model->setStringList(QStringList(info.fileName()));
        m_loading->setHidden(false);

        if (inputWeb)inputWeb->deleteLater();
    });
}

void MusicShow::changeMouseIcon(char ch)
{
    switch (ch) {
    case 'A'://直箭
        this->setCursor(QCursor(Qt::UpArrowCursor));
        break;
    case 'B'://十字
        this->setCursor(QCursor(Qt::CrossCursor));
        break;
    case 'C'://满十字
        this->setCursor(QCursor(Qt::SizeAllCursor));
        break;
    case 'D'://手指
        this->setCursor(QCursor(Qt::PointingHandCursor));
        break;
    case 'E'://手抓
        this->setCursor(QCursor(Qt::OpenHandCursor));
        break;
    case 'F'://手握
        this->setCursor(QCursor(Qt::ClosedHandCursor));
        break;
    case 'G'://等待
        this->setCursor(QCursor(Qt::WaitCursor));
        break;
    case 'H'://禁止
        this->setCursor(QCursor(Qt::ForbiddenCursor));
        break;
    case 'W'://隐藏
        this->setCursor(Qt::BlankCursor);
        break;
    default://正常
        this->setCursor(QCursor(Qt::ArrowCursor));
        break;
    }
}

void MusicShow::sliderStyle(bool isRed)
{
    if(isRed)
    {
        //滑块变红色
        m_horizontalSlider->setStyleSheet("QSlider::handle:horizontal{background: #DC143C}");
    }
    else
    {
        // 恢复默认颜色
        m_horizontalSlider->setStyleSheet("QSlider::handle:horizontal{background: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.5,stop:0.6 #45ADED, stop:0.778409 rgba(255, 255, 255, 255));width: 11px;margin-top: -3px;margin-bottom: -3px;border-radius: 5px;}");
    }
}

void MusicShow::listTurnVedio(bool isVideo)
{
    m_title->setHidden(isVideo);
    m_playInfo->setHidden(isVideo);
    // m_speedControl->setHidden(isVideo);
    // m_loading->setHidden(isVideo);
    m_lound->setHidden(isVideo);
    m_video->setHidden(!isVideo);
    m_view->setHidden(!isVideo);
    m_listView->setHidden(isVideo);
    //    if (isVideo) m_video->waitingFor(0);
    m_isVideo = isVideo;
}


//无边框拉伸
void MusicShow::region(const QPoint &cursorGlobalPoint)
{
    // 获取窗体在屏幕上的位置区域，tl为topleft点，rb为rightbottom点
    QRect rect = this->rect();

    //对角线两个点
    QPoint tl = mapToGlobal(rect.topLeft());
    QPoint rb = mapToGlobal(rect.bottomRight());

    //当前的光标位置
    int x = cursorGlobalPoint.x();
    int y = cursorGlobalPoint.y();

    if(tl.x() + PADDING >= x && tl.x() <= x && (tl.y() + PADDING) >= y && tl.y() <= y)
    {
        // 左上角
        m_direct = LEFTTOP;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));  // 设置鼠标形状
    }
    else if(x >= rb.x() - PADDING && x <= rb.x() && y >= (rb.y() - PADDING) && y <= rb.y())
    {
        // 右下角
        m_direct = RIGHTBOTTOM;
        this->setCursor(QCursor(Qt::SizeFDiagCursor));
    } else if(x <= tl.x() + PADDING && x >= tl.x() && (y >= rb.y() - PADDING) && y <= rb.y())
    {
        //左下角
        m_direct = LEFTBOTTOM;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if(x <= rb.x() && x >= (rb.x() - PADDING) && y >= tl.y() && y <= tl.y() + PADDING)
    {
        // 右上角
        m_direct = RIGHTTOP;
        this->setCursor(QCursor(Qt::SizeBDiagCursor));
    }
    else if(x <= tl.x() + PADDING && x >= tl.x())
    {
        // 左边
        m_direct = LEFT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if( x <= rb.x() && x >= (rb.x() - PADDING))
    {
        // 右边
        m_direct = RIGHT;
        this->setCursor(QCursor(Qt::SizeHorCursor));
    }
    else if(y >= tl.y() && y <= (tl.y() + PADDING))
    {
        // 上边
        m_direct = UP;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else if(y <= rb.y() && y >= (rb.y() - PADDING))
    {
        // 下边
        m_direct = DOWN;
        this->setCursor(QCursor(Qt::SizeVerCursor));
    }
    else {
        // 默认
        m_direct = NONE;
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
}


/*
bool MusicShow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
  MSG* msg = static_cast<MSG*>(message);
  if (WM_NCHITTEST == msg->message)
    {
      int xPos = GET_X_LPARAM(msg->lParam) - this->frameGeometry().x();
      int yPos = GET_Y_LPARAM(msg->lParam) - this->frameGeometry().y();
      if(this->childAt(xPos,yPos) == 0)
        *result = HTCAPTION;
      else
        return false;
      if(xPos > 0 && xPos < 8)
        *result = HTLEFT;
      else if(xPos > (this->width() - 8) && xPos < (this->width() - 0))
        *result = HTRIGHT;
      else if(yPos > 0 && yPos < 8)
        *result = HTTOP;
      else if(yPos > (this->height() - 8) && yPos < (this->height() - 0))
        *result = HTBOTTOM;
      else if(xPos > 18 && xPos < 22 && yPos > 18 && yPos < 22)
        *result = HTTOPLEFT;
      else if(xPos > (this->width() - 22) && xPos < (this->width() - 18) && yPos > 18 && yPos < 22)
        *result = HTTOPRIGHT;
      else if(xPos > 18 && xPos < 22 && yPos > (this->height() - 22) && yPos < (this->height() - 18))
        *result = HTBOTTOMLEFT;
      else if(xPos > (this->width() - 22) && xPos < (this->width() - 18) && yPos > (this->height() - 22) && yPos < (this->height() - 18))
        *result = HTBOTTOMRIGHT;
      return true;
    }
  return QWidget::nativeEvent(eventType, message, result);

}*/

// 音量调节
void MusicShow::on_lound_sliderMoved(int value)
{
    m_player->setVolume(value);
}

void MusicShow::onMuted(bool)
{
    isMute = !isMute;
    m_player->setMuted(isMute);
    if(isMute)
        m_actMute->setText(tr("二耳聪心"));
    else
        m_actMute->setText(tr("颜羞羞言"));
}

// 持续时间
void MusicShow::onDuration(qint64 duration)
{
    m_duration = duration / 1000;
    m_horizontalSlider->setMaximum(duration / 1000);
}

// 播放时间
void MusicShow::onPlayTimer(qint64 value)
{
    int currentInfo = value/1000;

    // 滚动条设置
    if (!m_horizontalSlider->isSliderDown())
    {
        m_horizontalSlider->setValue(currentInfo);
    }

    // 时间设置
    if (currentInfo || m_duration)
    {
        QTime currentTime((currentInfo/3600)%60, (currentInfo/60)%60, currentInfo%60, (currentInfo*1000)%1000);
        QTime totalTime((m_duration/3600)%60, (m_duration/60)%60, m_duration%60, (m_duration*1000)%1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        QString singTime = currentTime.toString(format) ;
        m_timeUp->display( singTime );
        m_playInfo->setText( totalTime.toString(format) );
    }

    // 歌词同步
    if (m_isLrc) m_songLrc->updateTime(value);

}

// 滚动条
void MusicShow::onSeek(int seek)
{
    m_horizontalSlider->setValue(seek);
    m_player->setPosition(seek*1000);
}

void MusicShow::onSlowDown()
{
    g_rate -= 0.1;
    emit m_player->playbackRateChanged(g_rate);
}

void MusicShow::onRecover()
{
    g_rate = 1.0;
    emit m_player->playbackRateChanged(g_rate);
}

void MusicShow::onQuickUp()
{
    g_rate += 1.0;
    emit m_player->playbackRateChanged(g_rate);
}

//指定播放的歌曲
void MusicShow::onSingTheSong(int index)
{
    m_fileList->setCurrentIndex(index);
    m_playing = m_fileList->currentMedia().canonicalUrl();
    if (!m_playing.isValid() || m_playing.isEmpty()) return;
    for (int i = 0; i < m_model->rowCount(); i++)
    {
        if (m_playing.fileName() == m_model->index(i).data().toString())
        {
            m_listView->setCurrentIndex(m_model->index(i));
        }
    }

    m_player->play();
}



// 窗口边框设置
void MusicShow::on_noBoardStyle()
{

    if(isNoBorder)
    {
        m_actBorders->setText("边疆万里");
        this->setWindowFlags(Qt::FramelessWindowHint|Qt::Widget);
    }
    else
    {
        m_actBorders->setText("万寿无疆");
        this->setWindowFlags(Qt::Widget);
    }
    this->activateWindow();
    this->show();
    isNoBorder = !isNoBorder;
}

void MusicShow::on_topWindow()
{

    if(m_isTop)
    {
        m_actTop->setText("拼醉花前");
        if (isNoBorder)
            this->setWindowFlags(Qt::WindowStaysOnBottomHint);
        else
            this->setWindowFlags(Qt::WindowStaysOnBottomHint|Qt::FramelessWindowHint);
    }
    else
    {
        m_actTop->setText("取消最前");
        if (isNoBorder)
            this->setWindowFlags(Qt::WindowStaysOnTopHint);
        else
            this->setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
    }
    this->show();
    m_isTop = !m_isTop;
}

// 播放模式
void MusicShow::on_playModel_clicked()
{
    DialogMx *addressGlg = new DialogMx(this);
    addressGlg->setGeometry(this->pos().x()+this->width()-120,this->pos().y()+this->height()-250,100,180);
    if(m_isTop)
        addressGlg->setWindowFlags(Qt::WindowStaysOnTopHint|Qt::Dialog|Qt::FramelessWindowHint);
    else
        addressGlg->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);

    addressGlg->setAttribute(Qt::WA_DeleteOnClose,true);
    addressGlg->setModal(true);
    addressGlg->showNormal();
    //m_layout->addWidget(addressGlg,4,5,1,1,Qt::AlignRight);

    ButtonMx* btnOnce = new ButtonMx("单曲播放",addressGlg);
    ButtonMx* btnCurLoop = new ButtonMx("单曲循环",addressGlg);
    ButtonMx* btnSequential = new ButtonMx("顺序播放",addressGlg);
    ButtonMx* btnListLoop = new ButtonMx("列表循环",addressGlg);
    ButtonMx* btnRandom = new ButtonMx("随机播放",addressGlg);

    QVBoxLayout* fLayout = new QVBoxLayout;
    fLayout->addWidget(btnOnce);
    fLayout->addWidget(btnCurLoop);
    fLayout->addWidget(btnSequential);
    fLayout->addWidget(btnListLoop);
    fLayout->addWidget(btnRandom);

    addressGlg->setLayout(fLayout);
    switch (m_fileList->playbackMode())
    {
    case QMediaPlaylist::CurrentItemOnce:
        btnOnce->setChecked(true);
        break;
    case QMediaPlaylist::CurrentItemInLoop:
        btnCurLoop->setChecked(true);
        break;
    case QMediaPlaylist::Sequential:
        btnSequential->setChecked(true);
        break;
    case QMediaPlaylist::Loop:
        btnListLoop->setChecked(true);
        break;
    case QMediaPlaylist::Random:
        btnRandom->setChecked(true);
        break;
    default:
        btnOnce->setChecked(true);
        addressGlg->deleteLater();
        break;
    }
    connect(btnOnce,&ButtonMx::chooseSIG,this,[=](){
        playModel(0);
        addressGlg->deleteLater();
    });
    connect(btnCurLoop,&ButtonMx::chooseSIG,this,[=](){
        playModel(1);
        addressGlg->deleteLater();
    });
    connect(btnSequential,&ButtonMx::chooseSIG,this,[=](){
        playModel(2);
        addressGlg->deleteLater();
    });
    connect(btnListLoop,&ButtonMx::chooseSIG,this,[=](){
        playModel(3);
        addressGlg->deleteLater();
    });
    connect(btnRandom,&ButtonMx::chooseSIG,this,[=](){
        playModel(4);
        addressGlg->deleteLater();
    });

}

void MusicShow::OnTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
    case QSystemTrayIcon::DoubleClick:
        if(this->isHidden())
            this->show();
        break;
    default:
        break;
    }
}

void MusicShow::onSigletonShow()
{
    if(m_isPlayer && m_isVideo)
    {
        if(isSigleton)
        {
            sigletonShow(false);
            m_actSigleton->setText("一幕了然");
        }
        else
        {
            sigletonShow(true);
            m_actSigleton->setText("客座莲花");
        }
        isSigleton = !isSigleton;
    }

}

void MusicShow::onSongShow()
{
    if(m_isShowLrc) {
        m_songLrc->setHidden(true);
        m_actLry->setText(tr("暗流涌动"));
    }else{
        m_songLrc->setHidden(false);
        m_actLry->setText(tr("英花逐流"));
    }
    m_isShowLrc = !m_isShowLrc;
}

void MusicShow::on_err(QMediaPlayer::Error error)
{
    Q_UNUSED(error)
    qDebug()<<"ERROR99";
    /*
    if(m_player)
      {
        delete m_player;
        m_player = nullptr;
      }
    m_player = new QMediaPlayer;
    m_player->setVideoOutput(m_view);*/

}

void MusicShow::onMediastatus(QMediaPlayer::MediaStatus status)
{
    switch (status)
    {
    case QMediaPlayer::EndOfMedia:
        qDebug()<<100;
        break;
    case QMediaPlayer::UnknownMediaStatus:
        qDebug()<<200;
        break;
    case QMediaPlayer::InvalidMedia:
        qDebug()<<300;
        break;
    case QMediaPlayer::NoMedia:
        qDebug()<<400;
        break;
    case QMediaPlayer::LoadingMedia:
        sigletonShow(false);//正常窗体
        qDebug()<<500;
        break;
    case QMediaPlayer::LoadedMedia:
        qDebug()<<600;
        m_playInfo->setText("播放信息");
        break;
    case QMediaPlayer::StalledMedia:
        qDebug()<<700;
        break;
    case QMediaPlayer::BufferingMedia:
        qDebug()<<800;
        break;
    case QMediaPlayer::BufferedMedia:
        qDebug()<<900;
        QThread::usleep(100);
        break;
    default:
        break;
    }

}
