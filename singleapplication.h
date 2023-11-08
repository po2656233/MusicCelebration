///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file     singleapplication.h
/// @brief    音乐界面展示
/// @author   陈志华
/// @date     2017-4-18
/// @version  1.0
/// @note     实现单例播放器
/// 说明：即一个终端只显示一个播放器
/// 涵盖：
/// 1、开机启动功能
/// 2、缓慢显示
/// 3、托盘模式
///////////////////////////////////////////////////////
#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

//#include <QtCore/qglobal.h>
#include <QObject>
#include <QApplication>

QT_BEGIN_NAMESPACE
class QLocalServer;
class QPropertyAnimation;
QT_END_NAMESPACE
class MusicShow;

class SingleApplication : public QApplication
{
  Q_OBJECT
public:
  explicit SingleApplication(int &argc, char **argv);
  ~SingleApplication();
  bool isRunning();                             // 是否已经有实例在运行
  void setMainWindow(MusicShow* mainShow);      // 需要传输实例对象
  void setAutoStart(bool bAutoStart);           // 开机启动
  void toSlowly();                              // 缓慢显示
  void toTray();                                // 托盘模式
private slots:
  // 有新连接时触发
  void newLocalConnection();

private:
  // 初始化本地连接
  void initLocalConnection();
  // 创建服务端
  void newLocalServer();

  bool m_bRunning;                  // 是否已经有实例在运行
  QLocalServer *m_localServer;      // 本地socket Server
  QPropertyAnimation *m_animation;  // 缓慢显示界面
  MusicShow *m_mainWindow;          // MainWindow指针
  QString m_serverName;             // 服务名称

};

#endif // SINGLEAPPLICATION_H
