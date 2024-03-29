///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file     musicshow.h
/// @brief    音乐界面展示
/// @author   陈志华
/// @date     2017-4-18
/// @version  1.0
/// @note     实现音乐的整体界面展示：
/// 涵盖：
/// 1\曲目录入 注:过滤重名
/// 2\曲目播放
/// 3\时间显示
/// 4\歌词显示
/// 5\播放模式
///////////////////////////////////////////////////////
#ifndef MUSICSHOW_H
#define MUSICSHOW_H

#include "mdk/Player.h"
#include <QWidget>
// #include <QMediaPlayer>
#include <QFileInfoList>
#include <QModelIndexList>
#include <QSystemTrayIcon>


QT_BEGIN_NAMESPACE
class QPushButton;
class QStringListModel;
class QMediaPlayer;
class QVideoWidget;
class QGridLayout;
class QVBoxLayout;
class QLCDNumber;
class QListView;
class QLabel;
class QSlider;
class QDial;
class QMenu;
class QWebView;
class QLineEdit;
class QGraphicsOpacityEffect;
QT_END_NAMESPACE

class VideoView;
class WebView;
class DialogMx;
class ButtonMx;
class BlinkBtn;
class VideoWidget;
class SpeedControl;
class MyLyric;
class QMDKPlayer;
class QMDKWidgetRenderer;


enum Direction
{
    UP = 0,
    DOWN = 1,
    LEFT,
    RIGHT,
    LEFTTOP,
    LEFTBOTTOM,
    RIGHTBOTTOM,
    RIGHTTOP,
    NONE
};


enum PlayerModule
{
    UNKNOW = 0,
    Once = 1,
    InLoop = 2,
    Sequential = 3,
    Loop = 4,
    Random = 5
};

class MusicShow : public QWidget
{
    Q_OBJECT
public:
    explicit MusicShow(QWidget *parent = 0);
    ~MusicShow();

    // 添加歌曲
    void addSong(QString songAddr,bool orderDesc=true);
    void addSongList(QStringList songAddrs);
    bool addWeb(QString webAddr,bool orderDesc=true);// 网络加载
    void addWebList(const QString& filePath);

    // 同步歌词
    void synchronyLrc(const QString &fileName);

    // 设置提示信息
    void setHint(QString fileName, bool isRightIn = true,int showtime = 30);

    // 加载记录文件
    void loadRecord();

    // 正播放的曲目
    QString getPlaying();
    int  getCurrentIndex();

    // 所有曲目
    QStringList getAllFiles(const QString& dir);

private:
    //  (需检测是否是视频)
    bool adjustShow();

protected:
    // 键盘事件——控制状态
    void keyPressEvent(QKeyEvent *event);
    // 鼠标事件——控制显示
    void mouseDoubleClickEvent(QMouseEvent *event);
    // 拖拽界面
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *event);
    // 右键弹框
    void contextMenuEvent(QContextMenuEvent *event);
    // 关闭事件
    void closeEvent(QCloseEvent *event);
    // void hideEvent(QHideEvent *event);
    // 音量控制
    void wheelEvent(QWheelEvent *event);
    // 事件过滤
    bool eventFilter(QObject *target, QEvent *event);

    //bool nativeEvent(const QByteArray& eventType, void *message, long *result);
private slots:
    // 加载资源
    void onLoading();
    // 文件载入
    void onLoadingDir();
    // 网络加载
    void onLoadingWeb();

    // 有无边框切换
    void onNoBoardStyle();
    void onTopWindow();

    // 选择歌曲
    // 播放指定歌曲
    void onSingTheSong(int index);
    void onSelectitem(const QModelIndex &index);
    void onSelectitem_singal(const QModelIndex &index);
    void onPlaySelect();

    // 播放
    void onPlay();
    // 暂停
    void onPause();
    // 停止
    void onStop();

    // 歌曲步进
    void onSeek(int seek);
    // 降速 或 上一首
    void onSlowDown();
    // 恢复步进 或 切换播放上下首
    void onRecover();
    // 快进 或 下一首
    void onQuickUp();

    // 持续时间
    void onDuration(qint64 duration);
    // 播放时间
    void onPlayTimer(qint64 value);
    // 音量调节
    void onLoundSliderMoved(int position);
    // 静音
    void onMuted(bool);

    // 播放模式
    void onPlayModelClicked();
    // 托盘模式
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
    // 单口播放
    void onSigletonShow();
    // 显示歌词
    void onSongShow();
    // 文本淡出
    void onOpacity();

    // // 状态变化
    void onStatus(mdk::State status);
    // // 媒体状态
    void onMediastatus(mdk::MediaStatus status);

    // 进度条
    void onTimeOut();

    // 清空列表
    void onContextmenu(const QPoint&);
    void onDeleteItem();
    void onCopyItem();
    void onClear();

    void onQuit();

    // 异常处理
    // void onErr(QMediaPlayer::Error error);

signals:
    void signalHide();

private:
    // 文件校验
    bool isSong(const QString& songName);//文件校验/是否歌曲
    bool isLive(const QString& songName);//是否直播
    bool isVideo(const QString& songName);//是否视频
    void saveLiveInfo(const QString& data,bool isBatch = false);// 保存直播网址信息
    void saveRecord();

    // 界面优化
    void region(const QPoint &cursorGlobalPoint);//矩形
    void changeMouseIcon(char ch);//改变鼠标形状
    void sliderStyle(bool isRed);//滑块样式
    void listTurnVedio(bool isVideo);//列表变视频
    void sigletonShow(bool isShow);

    // 子功能
    void previous(); // 上一首
    void next();//下一首


    // 界面成员
    //***基本信息
    Direction               m_direct;       //方向
    QLabel*                 m_playInfo;     //播放信息
    QLabel*                 m_hintInfo;     //提示信息
    QLabel*                 m_singnerInfo;  //歌手名称
    BlinkBtn*               m_title;        //标题
    SpeedControl*           m_speedControl; //播放速度控制
    QTimer*                 m_timer;        //定时器
    QTimer*                 m_timerSlider;  //同步进度条定时器

    //***曲目状态
    QDial*                  m_lound;    //声音
    QLCDNumber*             m_timeUp;   //时间
    QPushButton*            m_loading;  //加载
    QPushButton*            m_paused;   //暂停
    QPushButton*            m_play;     //播放
    QPushButton*            m_stop;     //停止
    QSlider*                m_horizontalSlider; // 进度条

    //***播放模式
    QPushButton*            m_playMode; //播放模式

    //***播放资源
    QListView*              m_listView; //播放列表
    // VideoView*              m_videoView;//视频播放
    MyLyric*                m_songLrc;  //歌词显示
    QStringListModel*       m_model;    //列表信息
    QLabel*                 m_waiting; // 等待
    QMDKWidgetRenderer*     m_render;   // 当前播放渲染
    QMDKPlayer*             m_player;

    //***界面优化
    QGridLayout*            m_layout;     // 布局
    QMenu*                  m_popMenu;    // 右键弹框
    QAction*                m_actQuit;    // 退出
    QAction*                m_actBorders; // 边框
    QAction*                m_actTop;     // 置顶
    QAction*                m_actHide;    // 隐藏
    QAction*                m_actMute;    // 静音
    QAction*                m_actPlay;    // 播放
    QAction*                m_actCopy;    // 拷贝
    QAction*                m_actLry;     // 显示歌词
    QAction*                m_actDelete;  // 删除
    QAction*                m_actClear;   // 清空
    QAction*                m_actSigleton;// 单窗体
    WebView*                m_networdShow;// 网页
    QGraphicsOpacityEffect* m_effect;     // 提示效果控制
    // 数据成员
    //***基础信息
    bool                    m_isFirst:1;
    bool                    m_isTop:1;
    bool                    m_isPlayer:1;
    bool                    m_isLrc:1;
    bool                    m_isShowLrc:1;
    bool                    m_isVideo:1;
    bool                    m_isNext:1;
    float                   m_opaclevel;
    int                     m_preIndex;

    qint64                  m_duration;         // 时间周期
    QString                 m_playing;          // 当前播放的文件
    QPoint                  m_dragPosition;     // 窗口移动拖动时需要记住的点
    QString                 m_songsDir;         // 导入的音视频目录
    QString                 m_recordFile;       // 记录文件
    QString                 m_configFile;       // 配置信息
    QFileInfoList           m_songList;         // 文件列表
    PlayerModule            m_enModule;         // 播放模式

    QList<int>              m_preIndexs;        // 记录点播的索引
    QMap<QString,QString>       m_mapAnotherName;   // 网址别名 加载成功时生效,不要与列表名重复,否则被过滤掉。 如百度|http://www.baidu.com


    //QMap<qint64, QString>   lrc_map;
    //QTimer*                 m_timer;  // 歌词定时器
};
#endif // MUSICSHOW_H
