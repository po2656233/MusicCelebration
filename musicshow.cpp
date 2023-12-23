///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file
/// @brief
/// @author
/// @date
/// @version  1.0
/// @note
///////////////////////////////////////////////////////
//#include <Python.h>
#include "musicshow.h"
#include "widgetObjName.h"
// #include "videoview.h"
#include "dialogmx.h"
#include "buttonmx.h"
#include "blinkbtn.h"
#include "speedcontrol.h"
#include "webview.h"
#include "lineeditmx.h"

#include "mylyric.h"
#include <qmdk/QMDKPlayer.h>
#include <qmdk/QMDKRenderer.h>

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
#include <QMovie>

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
#include <QClipboard>
#include <QTextCodec>
#include <QBuffer>

#include <QListView>
#include <QStringListModel>
#include <QPalette>
#include <QKeyEvent>

// #include <QNetworkAccessManager>
// #include <QNetworkReply>
#include <QThread>
#include <QDirIterator>
#include <QGraphicsOpacityEffect>
#include <QCryptographicHash>

#include <QDebug>

#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QtMultimediaWidgets/QVideoWidget>

using namespace MDK_NS;

QString encrypt(const QString &src);
QString decrypt(const QString &src);
QString xorEncryptDecrypt(QString src, const QChar key);
QString toUpperAndLower(QString src);
static qreal g_rate = 1.0;
static bool isNoBorder = false;
static bool isMute = false;
static bool isDir = true;//是否目录
static bool isLeftPressDown = false;//鼠标左键按下
static bool isSigleton = false;
bool isLoadNow = false; //是否正在加载中
bool isLighterIn = true;// 是否淡入显示(仅对提示信息)
char xorkey = 75;
// 歌曲
QStringList audioList = QStringList()<<"*.mp3"<<"*.wma"<<"*.wav"\
                                      <<"*.ape"<<"*.aac"\
                                      <<"*.asf"<<"*.flac";
// 视频
QStringList videoList = QStringList()<<"*.mp4"<<"*.mov"<<"*.mkv"\
                                      <<"*.mpeg"<<"*.mpeg1"<<"*.mpeg2"<<"*.mpeg4"\
                                      <<"*.rm"<<"*.rmvb"\
                                      <<"*.avi"<<"*.m3u8";

// 直播源
QStringList liveList = QStringList()<<"*.m3u8"<<"*.m3u"<<"*.php?"<<"http://"<<"https://";


#define PADDING 2 //边距


QString encrypt(const QString &src)
{
    // 大小写加密
    QByteArray text = src.toLocal8Bit();
    QByteArray by = text.toBase64();
    QString str = QString(by);
    return toUpperAndLower(str);

    // 异或加密
    //    QByteArray text = src.toLocal8Bit();
    //    QByteArray by = text.toBase64();
    //    QString str = QString(by);
    //    // 异或加密(钥匙加密) --- 注意：（这里的key是可以指定为任意字符的，相应的，解密也要同意字符才行）
    //    return xorEncryptDecrypt(str, xorkey);
}

QString decrypt(const QString &src)
{
    // 大小写解密
    QString str = toUpperAndLower(src);
    QByteArray text = str.toLocal8Bit();
    QByteArray by = text.fromBase64(text);
    return QString::fromLocal8Bit(by); // 注意：这里是处理中文用的
    // 异或解密
    //    QString str = xorEncryptDecrypt(src, xorkey); //异或解密(钥匙解密)
    //    QByteArray text = str.toLocal8Bit();
    //    QByteArray by = text.fromBase64(text);
    //    return QString::fromLocal8Bit(by); // 注意：这里是处理中文用的
}
// 加解密都用此方法
QString toUpperAndLower(QString src)
{
    for(int i = 0; i < src.count(); i++) {
        QChar curC = src.at(i);
        if(curC.isUpper()) {
            curC = curC.toLower();
        } else if(curC.isLower()){
            curC = curC.toUpper();
        }
        src[i] = curC;
    }
    return src;
}
QString xorEncryptDecrypt(QString src, const QChar key)
{
    for(int i = 0; i < src.count(); i++) {
        src[i] = src.at(i).toLatin1() ^ key.toLatin1();
    }
    return src;
}

MusicShow::MusicShow(QWidget *parent) :
    QWidget(parent),m_isFirst(true),m_isTop(false),
    m_isLrc(false),m_isShowLrc(true),
    m_isVideo(false),m_isNext(true),
    m_opaclevel(0.0),m_duration(0),
    m_dragPosition(QPoint(0,0)){

    // 保存歌曲的目录
    m_songsDir = QCoreApplication::applicationDirPath()+"\\songs";
    QDir dir(m_songsDir);
    if(!dir.exists())dir.mkdir(m_songsDir);
    m_recordFile = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("live.dat");
    
    m_hintInfo = new QLabel(tr("祝君好心情")); //播放信息
    m_playInfo = new QLabel(tr("天涯海角")); //播放信息
    m_title = new BlinkBtn("高山流水"); //标题
    m_speedControl = new SpeedControl();//进度标题
    m_timer = new QTimer(); //定时器
    m_timerSlider = new QTimer();

    m_playMode = new QPushButton("PLAY MODE"); //播放模式
    m_loading = new QPushButton("START");  //加载
    m_paused = new QPushButton("PAUSE");   //暂停
    m_play = new QPushButton("PLAY");     //播放
    m_stop = new QPushButton("STOP");     //停止
    
    m_model = new QStringListModel();
    m_listView = new QListView();     // 播放列表
    m_waiting = new QLabel();         // 等待动画
    // m_videoView = new VideoView();    // 视频画面
    m_songLrc = new MyLyric(this);    // 歌词显示
    m_render = new QMDKWidgetRenderer();
    m_player = new QMDKPlayer();


    m_lound = new QDial();            // 声音
    m_timeUp = new QLCDNumber();      // 时间
    m_horizontalSlider = new QSlider(Qt::Horizontal);//进度条
    
    m_networdShow = new WebView(this);
    m_layout = new QGridLayout();//布局
    m_popMenu = new QMenu();//右键弹出的菜单
    m_actBorders = new QAction(tr("万寿无疆"),this);//右键无边框显示
    m_actQuit = new QAction(tr("闭月羞花"),this);//右键退出
    m_actTop = new QAction(tr("独步青云"),this);//置顶
    m_actHide = new QAction(tr("韬光养晦"),this);//隐藏
    m_actMute = new QAction(tr("鸦默雀静"),this);//静音
    m_actPlay = new QAction(tr("一曲肝肠"),this);//播放
    m_actCopy = new QAction(tr("一劳永逸"),this);//复制
    m_actLry = new QAction(tr("无言以对"),this);//隐藏歌词
    m_actDelete = new QAction(tr("一笔勾销"),this);//删除子项
    m_actClear = new QAction(tr("一扫而空"),this);//清空列表
    m_actSigleton = new QAction(tr("一幕了然"),this);//单窗体
    m_effect = new QGraphicsOpacityEffect(this);//效果控制
    m_enModule = Sequential;

    m_hintInfo -> setGraphicsEffect(m_effect);
    this->setMouseTracking(true); // 希望获取到鼠标移动时，光标靠近的位置。
    //    this->grabKeyboard();[弃用]
    this->setWindowFlags(Qt::Window|Qt::FramelessWindowHint|Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint|Qt::WindowMaximizeButtonHint);

    m_title->setFixedHeight(72);
    m_playInfo->setFixedHeight(72);

    // 布局
    this->setLayout(m_layout);
    this->resize(800,500);

    //    m_layout->setAlignment(Qt::AlignAbsolute);
    //m_layout->addWidget(m_songLrc,1,0,1,1);//歌词

    m_layout->addWidget(m_loading,5,0,1,1);
    m_layout->addWidget(m_speedControl,6,0,1,1);
    m_layout->addWidget(m_title,0,1,1,4);
    // m_layout->addWidget(m_videoView,0,1,5,5);
    m_layout->addWidget(m_waiting,0,1,5,5);
    m_layout->addWidget(m_listView,1,1,4,4);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addWidget(m_play);
    hLayout->addWidget(m_paused);
    hLayout->addWidget(m_stop);
    m_layout->addLayout(hLayout,5,1,1,4);
    m_layout->addWidget(m_horizontalSlider,6,1,1,4);
    m_layout->addWidget(m_hintInfo,0,0,3,1);
    m_layout->addWidget(m_playInfo,0,5,1,1,Qt::AlignHCenter);
    m_layout->addWidget(m_lound,1,5,4,1,Qt::AlignVCenter);
    m_layout->addWidget(m_playMode,5,5,1,1);
    m_layout->addWidget(m_timeUp,6,5,1,1);


    // 设置水平间距 设置垂直间距 设置外间距
    //    m_layout->setHorizontalSpacing(10);
    //    m_layout->setVerticalSpacing(10);
    //    m_layout->setContentsMargins(10, 10, 10, 10);
    
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

    
    // 快捷键
    m_play->setShortcut(QKeySequence(Qt::Key_Space));
    m_paused->setShortcut(QKeySequence(Qt::Key_P));
    m_stop->setShortcut(QKeySequence(Qt::Key_S));
    m_loading->setShortcut(QKeySequence(Qt::Key_O));
    m_actQuit->setShortcut(QKeySequence(Qt::Key_F4));
    //    m_actCopy->setShortcut(QKeySequence(Qt::Key_Copy));
    m_playMode->setShortcut(QKeySequence(Qt::Key_M));
    
    // 字体编排
    m_hintInfo->setWordWrap(true);
    m_hintInfo->setAlignment(Qt::AlignTop|Qt::AlignCenter);
    QString word = m_hintInfo->text();
    m_hintInfo->setText(word.split("").join("\n"));

    // 设置字体颜色
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    m_hintInfo->setPalette(pa);
    m_hintInfo->setFont(QFont("FZShuTi", 18));
    pa.setColor(QPalette::WindowText,QColor("#9370DB"));
    m_playInfo->setPalette(pa);
    m_playInfo->setFont(QFont("FangSong", 16));

    pa.setColor(QPalette::ButtonText,QColor(220,20,60));
    m_loading->setPalette(pa);
    pa.setColor(QPalette::ButtonText,QColor(125,0,125));
    m_playMode->setPalette(pa);
    pa.setColor(QPalette::ButtonText,QColor(224,255,255));
    m_play->setPalette(pa);
    m_paused->setPalette(pa);
    m_stop->setPalette(pa);

    
    QFont font = m_loading->font();
    font.setBold(true);
    m_loading->setFont(font);
    m_playMode->setFont(font);
    m_play->setFont(font);
    m_paused->setFont(font);
    m_stop->setFont(font);
    
    // 播放器
    // m_videoView->setHidden(true);
    // m_video->setVideo(m_mdkView);
    // m_video->setHidden(true);
    m_render->setSource(m_player);
    m_player->setDecoders(QStringList()<<"MFT:d3d=11"<<"CUDA"<<"hap"<<"D3D11"<<"DXVA");
    m_render->hide();

    //    m_view->setPlayer(m_player);

    m_lound->setValue(50);
    m_lound->setNotchesVisible(true);
    m_waiting->hide();
    QMovie *movie = new QMovie(tr(":/img/waiting.gif"));
    m_waiting->setMovie(movie);
    // m_waiting->setStyleSheet("background-image: url(background.png); background-repeat: repeat;");
    m_waiting->setScaledContents(true);
    movie->start();
    m_listView->setWordWrap(true);
    m_listView->setModel(m_model);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listView->installEventFilter(this);
    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_preIndex = -1;
    m_preIndexs.clear();

    // 关联信号和槽
    //加载文件
    connect(m_loading,SIGNAL(clicked()),this,SLOT(onLoading()));
    //点击播放
    connect(m_play,SIGNAL(clicked(bool)),this, SLOT(onPlay()));
    //点击暂停
    connect(m_paused,SIGNAL(clicked(bool)),this,SLOT(onPause()));
    //点击停止
    connect(m_stop,SIGNAL(clicked(bool)),this,SLOT(onStop()));
    
    //文件列表
    connect(m_listView,SIGNAL(pressed(QModelIndex)),this,SLOT(onSelectitem_singal(QModelIndex)));
    connect(m_listView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onSelectitem(QModelIndex)));

    //进度条
    connect(m_horizontalSlider, SIGNAL(sliderMoved(int)), this, SLOT(onSeek(int)));
    //声音
    connect(m_lound,SIGNAL(valueChanged(int)),this, SLOT(onLoundSliderMoved(int)));
    
    //播放模式
    connect(m_playMode,SIGNAL(clicked(bool)),this,SLOT(onPlayModelClicked()));


    //右键弹框
    connect(m_actHide, &QAction::triggered, this, &MusicShow::close);
    connect(m_actMute, SIGNAL(triggered(bool)), this, SLOT(onMuted(bool)));
    connect(m_actPlay, &QAction::triggered, this, &MusicShow::onPlaySelect);
    connect(m_actBorders, &QAction::triggered, this, &MusicShow::onNoBoardStyle);
    connect(m_actTop, &QAction::triggered, this, &MusicShow::onTopWindow);
    connect(m_actSigleton, &QAction::triggered, this, &MusicShow::onSigletonShow);
    connect(m_actLry, &QAction::triggered, this, &MusicShow::onSongShow);
    connect(m_actCopy, &QAction::triggered, this, &MusicShow::onCopyItem);
    connect(m_actDelete, &QAction::triggered, this, &MusicShow::onDeleteItem);
    connect(m_actClear, &QAction::triggered, this, &MusicShow::onClear);
    connect(m_actQuit,SIGNAL(triggered()),qApp,SLOT(quit()));
    connect(m_listView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextmenu(QPoint)));

    //播放速度
    connect(m_speedControl,SIGNAL(sigSlowDown()),this,SLOT(onSlowDown()));
    connect(m_speedControl,SIGNAL(sigRecover()),this,SLOT(onRecover()));
    connect(m_speedControl,SIGNAL(sigQuickUp()),this,SLOT(onQuickUp()));

    // 窗口模式切换
    connect(m_title,SIGNAL(doubleClicked()),this,SLOT(showFullScreen()));
    connect(m_title,SIGNAL(normalShow()),this,SLOT(showNormal()));

    // 文本淡出
    connect(m_timer,SIGNAL(timeout()),this,SLOT(onOpacity()));
    m_timer->start(30);
    connect(m_timerSlider,SIGNAL(timeout()),this,SLOT(onTimeOut()));

    loadRecord();
}

MusicShow::~MusicShow()
{
    saveRecord();
    if(m_model)
    {
        delete m_model;
        m_model = nullptr;
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
    if(m_waiting){
        m_waiting->clear();
        delete m_waiting;
        m_waiting = nullptr;
    }

    // if(m_videoView){
    //     delete m_videoView;
    //     m_videoView = nullptr;
    // }
    if(m_player){
        delete m_player;
        m_player = nullptr;
    }
    if(m_render){
        delete m_render;
        m_render = nullptr;
    }
    if(m_songLrc)
    {
        delete m_songLrc;
        m_songLrc = nullptr;
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
    if(m_speedControl){
        delete m_speedControl;
        m_speedControl = nullptr;
    }
    if(m_title)
    {
        delete m_title;
        m_title = nullptr;
    }
    if(m_effect){
        delete m_effect;
        m_effect = nullptr;
    }
    if(m_hintInfo)
    {
        delete m_hintInfo;
        m_hintInfo = nullptr;
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

    if(m_timer)
    {
        m_timer->destroyed();
    }
    if(m_timerSlider){
        m_timerSlider->destroyed();
    }
}

// 添加歌曲
void MusicShow::addSong(QString songAddr,bool orderDesc)
{
    QStringList songsList = orderDesc?QStringList()<<songAddr<<m_model->stringList():m_model->stringList()<<songAddr;
    songsList.removeDuplicates();
    m_model->setStringList(songsList);
}

// 添加歌曲列表
void MusicShow::addSongList(QStringList songAddrs)
{
    songAddrs = m_model->stringList()<<songAddrs;
    songAddrs.removeDuplicates();
    m_model->setStringList(songAddrs);
}

bool MusicShow::addWeb(QString origData, bool orderDesc)
{
    if (origData.isEmpty()){
        m_loading->setHidden(false);
        return false;
    }
    QString webAddr = origData;
    // 解析地址
    webAddr.replace(" ","|");
    webAddr.replace(",","|");
    QStringList webInfo = webAddr.split("|");
    QString anotherName = "";
    if(1<webInfo.length()){
        anotherName = webInfo[0];
        webAddr = webInfo[1];
        m_mapAnotherName[anotherName] = QUrl(webAddr).toString();
    }

    // 检测是否是直播源
    if(isLive(webAddr)){
        // 播放直播源
        // m_fileList->addMedia(QUrl(webAddr));
        anotherName.isEmpty()?addSong(webAddr,orderDesc):addSong(anotherName,orderDesc);
        // 是否保存纪录
        this->setHint(tr("直播源 加载成功!"));
        saveLiveInfo(origData);
        return true;
    }


    // 适配地址
    if (webAddr.left(4) != "http")webAddr = "http://"+webAddr;

    // 获取歌曲
    QString singName = m_networdShow->setUrl(webAddr,m_songsDir);
    isLoadNow = false;
    if(singName.isEmpty()){
        this->setHint(tr("非mp3网址"));
        return false;
    }

    // 添加入曲目列表
    // m_fileList->clear();
    QFileInfo info(singName);
    // m_fileList->addMedia( QUrl(info.absoluteFilePath()));
    anotherName.isEmpty()?addSong(info.fileName(),orderDesc):addSong(anotherName,orderDesc);

    this->setHint(tr("加载成功"));
    saveLiveInfo(origData);
    return true;
}

void MusicShow::addWebList(const QString &filePath)
{
    //    if(toSave && QFileInfo(filePath).absoluteFilePath() == QFileInfo(m_recordFile).absoluteFilePath())return;
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly )) {
        QTextStream in(&file);
        in.setGenerateByteOrderMark(true);    //这句是重点改成bom格式
        in.setCodec("UTF-8");
        while (!in.atEnd()) {
            // 处理每一行数据
            addWeb(in.readLine(),false);
        }
        file.close();
    }
}


QString MusicShow::getPlaying()
{
    QString songName;
    for (int i = 0; i < m_model->rowCount(); ++i) {
        songName = m_model->index(i).data().toString();
        if(!m_mapAnotherName[songName].isNull() && !m_mapAnotherName[songName].isEmpty())songName = m_mapAnotherName[songName];
        if(songName == m_playing)return m_model->index(i).data().toString();
    }
    return m_playing;
}

void MusicShow::previous()
{
    if(m_player)m_player->stop();
    m_preIndexs = m_preIndexs.toSet().toList() ;
    if(m_preIndexs.size() < m_preIndex)m_preIndex = m_preIndexs.size();
    m_preIndex--;
    if(m_preIndex < 0)m_preIndex = 0;
    if(m_preIndex < m_preIndexs.size())onSingTheSong(m_preIndexs.at(m_preIndex));
}

void MusicShow::next()
{
    if(m_player)m_player->stop();
    switch (m_enModule) {
    case Once:
        break;
    case InLoop:
        adjustShow();
        break;
    case Sequential:
    {
        int index = getCurrentIndex();
        if(m_model->rowCount()<=index+1)return;
        onSingTheSong(++index);
    }
    break;
    case Loop:
    {
        int index = getCurrentIndex();
        index = (m_model->rowCount()<=index+1)?0:index+1;
        onSingTheSong(index);
    }
    break;
    case Random:
    {
        static QTime static_t = QTime::currentTime();
        QTime time = QTime::currentTime();
        int sec = static_t.msecsTo(time);
        qsrand(sec);
        int index = qrand()% m_model->rowCount();
        int curIndex = getCurrentIndex();
        if(index == curIndex){
            index++;
            if(m_model->rowCount()<=index ){
                index = 0;
            }
            if(curIndex == index)index = 1;
        }
        onSingTheSong(index);
    }
    break;
    default:
        return;
    }

}


int MusicShow::getCurrentIndex() const
{
    if(!m_playing.isEmpty()){
        for (int i = 0; i < m_model->rowCount(); i++) {
            if(m_playing == m_model->index(i).data().toString()){
                return i;
            }
        }
    }
    return m_listView->selectionModel()->currentIndex().row();
}

void MusicShow::sigletonShow(bool isShow)
{
    m_listView->setHidden(isShow);
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

void MusicShow::saveLiveInfo(const QString &data, bool isBatch)
{
    QFile file(m_recordFile);
    // 打开文件，读写与追加
    if(file.open(QIODevice::ReadWrite|QIODevice::Text| QIODevice::Append)){
        file.seek(0);
        if(isBatch){
            QString content = decrypt(file.readAll());
            if(content.contains(data))return file.close();
        }else{
            while (!file.atEnd()) {
                QString content = decrypt(file.readLine());      //整行读取
                if(content.contains(data)) return file.close();
            }
        }
        // 加密并换行
        QTextStream liveData(&file);
        liveData.setGenerateByteOrderMark(true);    //这句是重点改成bom格式
        liveData.setCodec("UTF-8");
        liveData << encrypt(data) << Qt::endl;
    }
    // 关闭文件, 保存数据
    file.close();
}

bool MusicShow::isSong(const QString &songName)
{
    if(songName.isNull()||songName.isEmpty())return false;

    foreach (QString audio, audioList) {
        if(songName.contains(audio.replace("*",""))){
            if(!m_isFirst)synchronyLrc(songName);
            return true;
        }
    }
    return false;
}
// 是否是直播源
bool MusicShow::isLive(const QString &songName)
{
    foreach (QString live, liveList) {
        if(songName.contains(live.replace("*",""))){
            return true;
        }
    }
    return false;
}

// 是否是视频
bool MusicShow::isVideo(const QString &songName)
{
    foreach (QString video, videoList) {
        if(songName.contains(video.replace("*",""))){
            return true;
        }
    }
    return false;
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
    fileList<<audioList<<videoList<<liveList;
    dir.setNameFilters(fileList);


    // 读取文件信息列表
    //    m_songList = dir.entryInfoList(fileList, QDir::Files|QDir::AllEntries);
    //     fileList.clear();
    // 单层目录下的文件
    //    foreach (QFileInfo info, m_songList)
    //    {
    //        fileList<<info.fileName();
    //        m_fileList->addMedia( QUrl(info.absoluteFilePath()) );
    //    }


    // 获取子孙目录下所有文件
    QDirIterator dir_iterator(strDir,fileList,QDir::Files | QDir::AllEntries,QDirIterator::Subdirectories);
    fileList.clear();
    //    bool isOnce = true;
    while(dir_iterator.hasNext())
    {
        //        if(isOnce){
        //            m_fileList->clear();
        //            isOnce = false;
        //        }
        dir_iterator.next();
        QFileInfo file_info = dir_iterator.fileInfo();
        fileList<<file_info.fileName();
        addSong(file_info.fileName(),false);

        m_mapAnotherName[file_info.fileName()] = QUrl(file_info.absoluteFilePath()).toString();
    }


    //    if(!fileList.isEmpty()){
    //        // 清空之前的
    //        m_songList.clear();
    //        addSongList(fileList);
    //    }
    return fileList;
}

void MusicShow::synchronyLrc(const QString &fileName)
{
    qDebug()<<"当前曲目:"<<fileName;
    m_songLrc->stopLrcMask();
    m_songLrc->clear();
    m_isLrc = m_songLrc->loadLrc(&fileName);

    // 提示信息 展示歌曲
    QFileInfo info(fileName);
    QString songName = info.fileName();
    int size = songName.length()-4;
    if(0<size - 11){
        size = 8;
    }
    setHint(songName.left(size),false);

    // 展示歌词
    if (!m_isLrc){
        m_songLrc->setText(info.fileName());
    }
    if(m_isShowLrc) m_songLrc->show();
}

void MusicShow::setHint(QString hint, bool isRightIn, int showtime)
{
    m_timer->stop();
    m_hintInfo->clear();
    m_effect->setOpacity(1);
    m_effect->update();

    // 控制文本展示数目
    QString apostrophe = "";
    int size = hint.length();
    if(0 < size - 11){
        apostrophe = "...";
        size = 8;
    }

    m_hintInfo->setText(hint.left(size).split("", QString::SkipEmptyParts).join("\n"));
    m_hintInfo->show();
    qDebug()<<"设置提示:"<<hint;
    if(isRightIn){
        isLighterIn = isRightIn;
        m_opaclevel = 0;
        m_timer->start(showtime);
    }
}

void MusicShow::loadRecord()
{
    // 首次加载记录文件
    QFile file(m_recordFile);
    if (file.open(QIODevice::ReadOnly )) {
        QTextStream in(&file);
        in.setGenerateByteOrderMark(true);    //这句是重点改成bom格式
        in.setCodec("UTF-8");
        QString origData;
        while (!in.atEnd()) {
            // 处理每一行数据
            origData = decrypt( in.readLine());
            if (origData.isEmpty()){
                continue;
            }
            QString webAddr = origData;
            // 解析地址
            webAddr.replace(" ","|");
            webAddr.replace(",","|");
            QStringList webInfo = webAddr.split("|");
            QString anotherName = "";
            if(1<webInfo.length()){
                anotherName = webInfo[0];
                webAddr = webInfo[1];
                m_mapAnotherName[anotherName] = QUrl(webAddr).toString();
            }

            // 是否是歌曲
            if(isSong(webAddr)){
                QFileInfo info(webAddr);
                webAddr = info.fileName();
            }
            anotherName.isEmpty()?addSong(webAddr,false):addSong(anotherName,false);
        }
        file.close();
    }
    adjustShow();
    m_isFirst = false;
}


void MusicShow::saveRecord()
{
    QString songName;
    QFile file(m_recordFile);
    if (file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QTextStream textStream(&file);
        textStream.setGenerateByteOrderMark(true);    //这句是重点改成bom格式
        textStream.setCodec("UTF-8");
        for (int i = 0;i<m_model->rowCount();i++ ) {
            songName = m_model->index(i).data().toString();
            if(!m_mapAnotherName[songName].isNull() && !m_mapAnotherName[songName].isEmpty()){
                songName+=","+m_mapAnotherName[songName];
            }
            textStream<<encrypt(songName)<<Qt::endl;
        }
        file.flush();
        file.close();
    }
}


bool MusicShow::adjustShow()
{
    QModelIndex indexData = m_listView->currentIndex();
    QString songName = indexData.data().toString();
    if(songName.isNull() || songName.isEmpty())return false;

    int index = indexData.row();
    qDebug()<<"当前播放的"<<songName <<" 索引:"<<index;
    if(!m_mapAnotherName[songName].isEmpty()){
        songName = m_mapAnotherName[songName];
    }
    bool issong = isSong(songName);
    if(m_player == nullptr || 0 != songName.compare(m_player->currentMedia())){
        if(m_player){
            disconnect(m_player, SIGNAL(signalMediaStatusChanged(mdk::MediaStatus)), this, SLOT(onMediastatus(mdk::MediaStatus)));
            disconnect(m_player, SIGNAL(signalStateChanged(mdk::State)), this, SLOT(onStatus(mdk::State)));
            m_player->deleteLater();
        }

        m_player = new QMDKPlayer();

        m_render->setSource(m_player);
        m_player->setDecoders(QStringList()<<"MFT:d3d=11"<<"CUDA"<<"hap"<<"D3D11"<<"DXVA");
        m_player->setMedia(songName);
        onPlayTimer(m_player->startTime());
        onDuration(m_player->duration());
        qDebug()<<"时间1"<<m_player->startTime()<<m_player->duration()<<m_player->position();

        m_player->setVolume(m_lound->value());
        m_player->play();
        //媒体状态
        connect(m_player, SIGNAL(signalMediaStatusChanged(mdk::MediaStatus)), this, SLOT(onMediastatus(mdk::MediaStatus)));
        connect(m_player, SIGNAL(signalStateChanged(mdk::State)), this, SLOT(onStatus(mdk::State)));


        m_timerSlider->stop();
        m_timerSlider->start(1000);
        if(!issong)m_waiting->show();
        m_render->hide();

        m_preIndexs.push_front(index);
        m_preIndex++;
    }else if(m_player->isPaused() || m_player->isStopped()){
        m_player->play();
        m_timerSlider->start(1000);
    }else{
        return false;;
    }


    m_playing = songName;
    bool isLiving = isLive(songName);
    m_horizontalSlider->setEnabled(!isLiving);

    m_listView->scrollTo(m_model->index(getCurrentIndex()));
    m_songLrc->stopLrcMask();
    m_songLrc->clear();
    listTurnVedio(!issong);
    if(m_isShowLrc && isLiving){
        m_songLrc->hide();
    }
    return true;
}


void MusicShow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Plus)
    {
        int val = m_lound->value();
        m_lound->setValue(val+1);
        event->accept();
    }else if (event->key() == Qt::Key_Minus)
    {
        int val = m_lound->value();
        m_lound->setValue(val-1);
        event->accept();
    }else if (event->key() == Qt::Key_P)
    {
        onPause();
        event->accept();
    }
    QWidget::keyPressEvent(event);
}

//指定播放的歌曲
void MusicShow::onSingTheSong(int index)
{
    qDebug()<<"onSingTheSong"<<index;
    if(index < 0)return;

    m_listView->setCurrentIndex(m_model->index(index));
    adjustShow();
    QThread::usleep(100);
}

// 选择列表当中的曲目
void MusicShow::onSelectitem(const QModelIndex &index)
{
    qDebug()<<"onSelectitem "<<index.row();
    m_listView->setCurrentIndex(index);
    adjustShow();
    return;
}

void MusicShow::onSelectitem_singal(const QModelIndex &index)
{
    //    qDebug()<<"onSelectitem_singal==: "<<index;
    m_listView->setToolTip(index.data().toString());
    changeMouseIcon('D');
}


void MusicShow::onPlaySelect()
{
    QModelIndex index = m_listView->currentIndex();
    qDebug()<<"onPlaySelect "<<index.row();
    adjustShow();
}

// // 播放状态
void MusicShow::onStatus(mdk::State status)
{
    //    onRecover(); //标准速率
    qDebug()<<"播放状态"<<int(status);
    switch (status)
    {
    case mdk::State::Playing:
    {
        m_isPlayer = true;
        sliderStyle(true);
        m_listView->setToolTip(m_playing);
    }
    break;
    case mdk::State::Paused:
    {
        sliderStyle(false);
        m_isPlayer = false;
    }
    break;
    case  mdk::State::Stopped:
    {
        m_isPlayer = false;
        sliderStyle(false);
    }
    break;
    default:

        break;
    }


}


// 双击全屏
void MusicShow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        m_listView->setMinimumWidth(3*m_play->minimumWidth());
        m_listView->setMaximumWidth(m_play->maximumWidth());
        if(windowState()!=Qt::WindowFullScreen){
            if(!m_render->isHidden()){
                m_render->showFullScreen();
            }
            setWindowState(Qt::WindowFullScreen);
        }else{
            setWindowState(Qt::WindowNoState);
            if(!m_render->isHidden()){
                m_render->showNormal();
            }
        }
    }

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
    // event->ignore();
    // QWidget::mousePressEvent(event);
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
            // if( isLeftPressDown && (m_isPlayer || m_isVideo) ) m_mdkView->setHidden(true);
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
        // listTurnVedio(m_isVideo);
    }

    event->ignore();
    QWidget::mouseReleaseEvent(event);
}

void MusicShow::contextMenuEvent(QContextMenuEvent *event)
{

    if(m_listView->geometry().contains(this->mapFromGlobal(QCursor::pos()))){
        return;
    }
    m_popMenu->clear();
    m_popMenu->addAction(m_actTop);
    //m_popMenu->addAction(m_actBorders);//有无边框
    m_popMenu->addAction(m_actHide);
    m_popMenu->addSeparator();    //分割线
    m_popMenu->addAction(m_actMute);
    m_popMenu->addAction(m_actLry);
    m_popMenu->addSeparator();    //分割线
    m_popMenu->addAction(m_actSigleton);
    m_popMenu->addSeparator();    //分割线
    m_popMenu->addAction(m_actQuit);

    //菜单出现的位置为当前鼠标的位置
    m_popMenu->setStyleSheet(tr("QMenu{background-color:#4169E1;}"));
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
        if(m_player)m_player->setVolume(numSteps);

    }
    event->accept();      //接收该事件
    return QWidget::wheelEvent(event);
}

bool MusicShow::eventFilter(QObject *target, QEvent *event)
{
    if(target == m_listView)
    {
        // 键盘事件
        if (event->type() == QEvent::KeyPress)
        {
            int key = ((QKeyEvent *)event)->key();
            int val = m_lound->value();
            if (key== Qt::Key_Plus)
            {
                m_lound->setValue(val+1);

            }else if (key == Qt::Key_Minus)
            {
                m_lound->setValue(val-1);
            }
            return true;
        }
    }

    // standard event processing
    return QWidget::eventFilter(target, event);
}


void MusicShow::onLoading()
{
    DialogMx *loadDlg = new DialogMx(this);
    loadDlg->setGeometry(this->pos().x()+m_loading->x(),this->pos().y()+m_loading->y()-75-m_loading->height(),m_loading->width(),100);
    if(m_isTop)
        loadDlg->setWindowFlags(Qt::WindowStaysOnTopHint|Qt::Dialog|Qt::FramelessWindowHint);
    else
        loadDlg->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    
    loadDlg->setAttribute(Qt::WA_DeleteOnClose,true);
    loadDlg->setModal(true);
    loadDlg->showNormal();
    //m_layout->addWidget(loadDlg,4,5,1,1,Qt::AlignRight);
    
    ButtonMx* btnLocal = new ButtonMx("本地导入",loadDlg);
    ButtonMx* btnWeb = new ButtonMx("网络资源",loadDlg);
    // 字体样式
    QFont loadFt = QFont("STLiti", 10);
    QPalette loadPa;
    loadPa.setColor(QPalette::WindowText,QColor(200,0,138));
    
    btnLocal->setPalette(loadPa);
    btnLocal->setFont(loadFt);
    
    btnWeb->setPalette(loadPa);
    btnWeb->setFont(loadFt);
    
    QVBoxLayout* fLayout = new QVBoxLayout;
    fLayout->addWidget(btnLocal);
    fLayout->addWidget(btnWeb);
    loadDlg->setLayout(fLayout);

    btnLocal->setShortcut(QKeySequence(Qt::Key_Up));
    btnWeb->setShortcut(QKeySequence(Qt::Key_Down));

    connect(btnLocal, &ButtonMx::chooseSIG, this, [=](){
        delete loadDlg;
        onLoadingDir();
    });
    
    connect(btnWeb,&ButtonMx::chooseSIG, this, [=](){
        delete loadDlg;
        onLoadingWeb();
    });
    //    connect(loadDlg,&QDialog::accepted, this, [=](){
    //        delete loadDlg;
    //    });
    
    //  isDir?onLoadingDir():on_loading_single();
    //  isDir = !isDir;
    
}

void MusicShow::onLoadingDir()
{
    isDir = true;
    m_actTop->setText("至高无上");
    setHint(tr("加载中"),false);
    setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    showNormal();
    QString dir = QFileDialog::getExistingDirectory(Q_NULLPTR, QString("请选择歌曲目录"), m_songsDir,QFileDialog::DontUseNativeDialog|QFileDialog::ReadOnly|QFileDialog::ShowDirsOnly);
    qDebug()<<dir;
    if(dir.isEmpty()){
        setHint(tr("取消加载"));
        return;
    }
    m_songsDir = dir;
    if(getAllFiles(dir).isEmpty() ){
        setHint(tr("没有内容可加载"));
        return;
    }
    setHint(tr("完成加载"));
    qDebug()<<"成功";
}


void MusicShow::onLoadingWeb()
{
    // 开始按钮置灰
    QPalette pa = m_loading->palette();
    m_loading->setPalette(QPalette(QPalette::WindowText, Qt::gray));
    m_loading->setEnabled(false);

    // 显示用户输入框
    LineEditMx* inputWeb = new LineEditMx(this);
    inputWeb->setFixedWidth(m_loading->width());
    inputWeb->setFixedHeight(m_loading->height());
    setHint(tr("网址或直播源"));
    inputWeb->setPlaceholderText("请录入歌曲网址");
    m_layout->addWidget(inputWeb,2,0,2,1);

    isLoadNow = false;
    connect(inputWeb,&LineEditMx::returnPressed,this,[=](){
        // 禁止频繁 回车
        if(isLoadNow)return;
        inputWeb->setEnabled(false);
        isLoadNow = !isLoadNow;
        // 停止当前播放
        if(isLive(m_playing) || isVideo(m_playing)){
            onStop();
        }

        // 加载网络资源
        QString webAddr = inputWeb->text();
        QString head = tr("file:///");
        if(webAddr.left(head.length()) == head){
            webAddr.replace(head,"");
        }

        if(QFileInfo(webAddr).isFile()){
            // 从文本中获取
            addWebList(webAddr);
        }else if (!addWeb(webAddr)){// 添加网址
            isLoadNow = false;
            inputWeb->setEnabled(true);
            return ;
        }

        // 发送结束信号
        emit inputWeb->over();
    });

    // 输入结束后还原 开始按钮
    connect(inputWeb,&LineEditMx::over,this,[=](){
        m_loading->setPalette(pa);
        m_loading->setEnabled(true);
        setHint("完成加载");
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
    m_layout->removeWidget(m_listView);
    if(isVideo){
        m_listView->setStyleSheet("border-image:url(:/img/bg1.jpg);background-color: #412550;background-position:center;background-repeat:no-repeat;");
        m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_listView->setFixedWidth(130);
        m_layout->addWidget(m_listView,0,0,1,1);
    }else{
        m_render->setHidden(true);
        m_listView->setStyleSheet("border-image:url(:/img/bg3.jpg);background-position:center;background-repeat:no-repeat;");
        m_listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        m_listView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_listView->setSizePolicy( QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
        m_listView->setMinimumWidth(3*m_play->minimumWidth());
        m_listView->setMaximumWidth(m_play->maximumWidth());
        m_layout->addWidget(m_listView,1,1,4,4);
    }
    if(m_render){
        if(m_layout->indexOf(m_render)< 0){
            m_layout->addWidget(m_render,0,1,5,5);
        }
        // m_render->setHidden(!isVideo);
    }
    m_listView->setSelectionRectVisible(true);
    m_title->setHidden(isVideo);
    m_playInfo->setHidden(isVideo);
    m_hintInfo->setHidden(isVideo);
    m_lound->setHidden(isVideo);
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
void MusicShow::onLoundSliderMoved(int value)
{
    m_lound->setValue(value);
    if(m_player)m_player->setVolume(value);
}

void MusicShow::onMuted(bool)
{
    isMute = !isMute;
    if(m_player)m_player->setMuted(isMute);
    if(isMute)
        m_actMute->setText(tr("逖听远闻"));
    else
        m_actMute->setText(tr("鸦雀无声"));
}

// 持续时间
void MusicShow::onDuration(qint64 duration)
{
    m_duration = duration / 1000;
    if(m_player)m_player->seek(m_duration);
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
    if(m_player)m_player->seek(seek*1000);

}

void MusicShow::onSlowDown()
{
    if(m_isNext || !m_horizontalSlider->isEnabled()){
        previous();
        return;
    }
    if(1.0 < g_rate){
        g_rate = 1.0;
    }
    g_rate -= 0.1;
    if (g_rate<=0.1){
        g_rate = 0.1;
    }
    if(m_player)m_player->setPlaybackRate(g_rate);
}

void MusicShow::onRecover()
{
    m_listView->scrollTo(m_model->index(getCurrentIndex()), QListView::PositionAtCenter);
    if(!m_horizontalSlider->isEnabled())return;

    if(!m_isNext){
        g_rate = 1.0;
        setHint("左前首 右下一首",false);
    }else{
        setHint("左慢 右快",false);
    }
    m_isNext = !m_isNext;
}

void MusicShow::onQuickUp()
{
    if(m_isNext|| !m_horizontalSlider->isEnabled()){
        next();
        return;
    }
    if(g_rate<1.0){
        g_rate = 1.0;
    }
    g_rate += 0.1;
    if (4.3<=g_rate){
        g_rate = 4.3;
    }
    if(m_player) m_player->setPlaybackRate(g_rate);
}




// 窗口边框设置
void MusicShow::onNoBoardStyle()
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

void MusicShow::onTopWindow()
{
    
    if(m_isTop)
    {
        m_actTop->setText("处尊居显");
        this->setWindowFlags(
            Qt::Window \
            |Qt::FramelessWindowHint\
            |Qt::WindowSystemMenuHint\
            |Qt::WindowMinimizeButtonHint\
            |Qt::WindowMaximizeButtonHint);
        //             this->setWindowFlags(Qt::WindowStaysOnBottomHint|Qt::FramelessWindowHint);
    }
    else
    {
        m_actTop->setText("平平无奇");
        if (isNoBorder)
            this->setWindowFlags(Qt::WindowStaysOnTopHint);
        else
        {
            this->setWindowFlags(
                Qt::WindowStaysOnTopHint \
                |Qt::FramelessWindowHint\
                |Qt::WindowSystemMenuHint\
                |Qt::WindowMinimizeButtonHint\
                |Qt::WindowMaximizeButtonHint);
            //            this->setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
        }

    }
    this->show();
    m_isTop = !m_isTop;
}

// 播放模式
void MusicShow::onPlayModelClicked()
{
    DialogMx *addressGlg = new DialogMx(this);
    addressGlg->setGeometry(this->pos().x()+m_playMode->x(),this->pos().y()+m_playMode->y()-178-m_playMode->height(),m_playMode->width(),180);
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
    btnRandom->setShortcut(QKeySequence(Qt::Key_0));
    btnListLoop->setShortcut(QKeySequence(Qt::Key_4));
    btnSequential->setShortcut(QKeySequence(Qt::Key_3));
    btnCurLoop->setShortcut(QKeySequence(Qt::Key_2));
    btnOnce->setShortcut(QKeySequence(Qt::Key_1));

    QVBoxLayout* fLayout = new QVBoxLayout;
    fLayout->addWidget(btnOnce);
    fLayout->addWidget(btnCurLoop);
    fLayout->addWidget(btnSequential);
    fLayout->addWidget(btnListLoop);
    fLayout->addWidget(btnRandom);


    addressGlg->setLayout(fLayout);
    switch (m_enModule)
    {
    case PlayerModule::Once:
        btnOnce->setChecked(true);
        break;
    case PlayerModule::InLoop:
        btnCurLoop->setChecked(true);
        break;
    case PlayerModule::Sequential:
        btnSequential->setChecked(true);
        break;
    case PlayerModule::Loop:
        btnListLoop->setChecked(true);
        break;
    case PlayerModule::Random:
        btnRandom->setChecked(true);
        break;
    default:
        btnOnce->setChecked(true);
        addressGlg->deleteLater();
        break;
    }
    connect(btnOnce,&ButtonMx::chooseSIG,this,[=](){
        m_enModule = PlayerModule::Once;
        addressGlg->deleteLater();
    });
    connect(btnCurLoop,&ButtonMx::chooseSIG,this,[=](){
        m_enModule = PlayerModule::InLoop;
        addressGlg->deleteLater();
    });
    connect(btnSequential,&ButtonMx::chooseSIG,this,[=](){
        m_enModule = PlayerModule::Sequential;
        addressGlg->deleteLater();
    });
    connect(btnListLoop,&ButtonMx::chooseSIG,this,[=](){
        m_enModule = PlayerModule::Loop;
        addressGlg->deleteLater();
    });
    connect(btnRandom,&ButtonMx::chooseSIG,this,[=](){
        m_enModule = PlayerModule::Random;
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
        m_actLry->setText(tr("英花逐流"));
    }else{
        m_songLrc->setHidden(false);
        m_actLry->setText(tr("暗流涌动"));
    }
    m_isShowLrc = !m_isShowLrc;
}

void MusicShow::onClear()
{
    if(m_player){
        m_player->stop();
        delete m_player;
        m_player = nullptr;
    }
    setHint("",false);
    m_preIndex = -1;
    m_preIndexs.clear();
    m_model->setStringList(QStringList());
    listTurnVedio(false);
    QFile::remove(m_recordFile);
}

// 播放
void MusicShow::onPlay()
{
    QModelIndex indexData = m_listView->selectionModel()->selectedIndexes().first();
    int index = indexData.row();
    qDebug()<<"onPlay index:"<<index;
    if(index < 0)return;

    m_listView->setCurrentIndex(indexData);
    m_waiting->setHidden(false);
    if(!adjustShow()){
        m_waiting->setHidden(true);
    }
}

// 暂停播放
void MusicShow::onPause()
{

    if(m_player && !m_player->isPaused()){
        m_player->pause();
        m_timerSlider->stop();
    }
}

// 停止播放
void MusicShow::onStop()
{
    if(m_player)m_player->stop();

    if(!getPlaying().isEmpty()){
        m_songLrc->clear();
        m_timerSlider->stop();
        setHint("播放停止");
    }
    m_waiting->setHidden(true);
    listTurnVedio( false);
}

void MusicShow::onOpacity()
{
    if(isLighterIn)  //淡入
    {
        m_opaclevel += 0.02;
        m_effect->setOpacity(m_opaclevel);
        if(m_opaclevel > 1){
            QEventLoop eventloop;
            QTimer::singleShot(3000, &eventloop, SLOT(quit()));	//持续3秒
            eventloop.exec();
            isLighterIn = false;
        }
    }
    else  //淡出
    {
        m_opaclevel -= 0.02;
        m_effect->setOpacity(m_opaclevel);
        if(m_opaclevel < 0){
            m_hintInfo -> hide();
            m_timer->stop();
        }
    }
}


void MusicShow::onTimeOut()
{
    if(m_player&& m_player->isPlaying() && 0 < m_player->position()){
        m_horizontalSlider->setValue(m_player->position()/1000);
        onPlayTimer(m_player->position());
    }
}

void MusicShow::onContextmenu(const QPoint &)
{
    if(!m_listView->selectionModel()->selectedIndexes().empty())
    {
        m_popMenu->clear();
        m_popMenu->setStyleSheet(tr("QMenu{background-color:#CCAC88;border:1px solid rgba(82,130,164,1);}"));
        m_popMenu->addAction(m_actPlay);
        m_popMenu->addSeparator();    //分割线
        m_popMenu->addAction(m_actCopy);
        m_popMenu->addSeparator();    //分割线
        m_popMenu->addAction(m_actDelete);
        m_popMenu->addSeparator();    //分割线
        m_popMenu->addAction(m_actClear);
        m_popMenu->exec(QCursor::pos());
    }
}

void MusicShow::onDeleteItem()
{
    QModelIndex index = m_listView->currentIndex();
    QString songName = m_model->data(index).toString();
    if(!m_mapAnotherName[songName].isNull() && !m_mapAnotherName[songName].isEmpty()){
        QString key = songName;
        songName = m_mapAnotherName[songName];
        m_mapAnotherName.remove(key);
    }


    int idx = index.row();
    m_model->removeRow(idx);
    m_preIndexs = m_preIndexs.toSet().toList() ;
    for (int i = 0; i < m_preIndexs.size(); ++i) {
        if(m_preIndexs[i] == idx){
            if(-1<m_preIndex && idx == m_preIndexs[m_preIndex]){
                m_preIndex = -1;
            }
            m_preIndexs.removeAt(i);
            break;
        }
    }


    if(songName == m_playing && m_player){
        m_player->stop();
    }
}

void MusicShow::onCopyItem()
{
    //获取选中的行列范围
    QModelIndexList indexList = m_listView->selectionModel()->selectedIndexes();
    if(indexList.isEmpty())
        return;

    //将数据放入剪贴板
    QApplication::clipboard()->setText(m_model->data(indexList.first()).toString());
}

// void MusicShow::onErr(QMediaPlayer::Error error)
// {
//     qDebug()<<"ERROR99"<<error;
//     setHint( "失败 "+getPlaying(),false);
//     QThread::usleep(100);
// }

void MusicShow::onMediastatus(mdk::MediaStatus status)
{ qDebug()<<"onMediastatus"<<status;
    if(!m_waiting->isHidden()){
        m_waiting->hide();
    }
    switch ((mdk::MediaStatus)status)
    {
    case mdk::MediaStatus::NoMedia:
        qDebug()<<"NoMedia"<<status;
        break;
    case mdk::MediaStatus::Unloaded:
        qDebug()<<"Unloaded"<<status;
        break;
    case mdk::MediaStatus::Loading:
        if(!isSong(m_playing))m_render->setHidden(false);
        qDebug()<<"Loading"<<status;
        break;
    case mdk::MediaStatus::Loaded:
        if(!isSong(m_playing))m_render->setHidden(false);
        qDebug()<<"Loaded"<<status;
        break;
    case mdk::MediaStatus::Prepared:
        qDebug()<<"Prepared"<<status;
        //        sigletonShow(false);//正常窗体
        // setHint(getPlaying()+" 加载中");
        break;
    case mdk::MediaStatus::Stalled:
        qDebug()<<"Stalled"<<status;
        // qDebug()<<600<<m_listView->currentIndex().row();

        break;
    case mdk::MediaStatus::Buffering:
        m_waiting->hide();
        qDebug()<<"Buffering"<<status;
        break;
    case mdk::MediaStatus::Buffered:
        m_waiting->hide();
        qDebug()<<"Buffered"<<status;
        break;
    case mdk::MediaStatus::End+1:
        qDebug()<<"End"<<status;
        m_playInfo->setText("天涯海角");
        next();
        break;
    case mdk::MediaStatus::Seeking:
        qDebug()<<"Seeking"<<status;
        break;
    case mdk::MediaStatus::Invalid:
        qDebug()<<"Invalid"<<status;
        setHint(" 加载失败 "+getPlaying());
        break;
    default:
        // qDebug()<<000000<<status;
        break;
    }

}

