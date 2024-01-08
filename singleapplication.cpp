///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file
/// @brief
/// @author
/// @date
/// @version  1.0
/// @note
///////////////////////////////////////////////////////
#include "singleApplication.h"
#include "musicShow.h"
#include "systemtraymx.h"

#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>
#include <QPropertyAnimation>
#include <QFileInfo>
#include <QLibrary>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#define REG_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

SingleApplication::SingleApplication(int &argc, char **argv)
  : QApplication(argc, argv)
  , m_bRunning(false)
  , m_localServer(nullptr)
  , m_animation(nullptr)
  , m_mainWindow(nullptr)
{
  // 取应用程序名作为LocalServer的名字
  m_serverName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
  //qDebug()<<serverName;
  initLocalConnection();
}

SingleApplication::~SingleApplication()
{
  if(m_animation) delete m_animation;
}

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 检查是否已經有一个实例在运行, true - 有实例运行， false - 没有实例运行
////////////////////////////////////////////////////////////////////////////////
bool SingleApplication::isRunning()
{
  return m_bRunning;
}

void SingleApplication::setMainWindow(MusicShow *mainShow)
{
  m_mainWindow = mainShow;
}

void SingleApplication::setAutoStart(bool bAutoStart)
{
  QString application_name = QApplication::applicationName();
  QSettings *settings = new QSettings(REG_RUN, QSettings::NativeFormat);
  if(bAutoStart)
    {
      QString application_path = QApplication::applicationFilePath();
      settings->setValue(application_name, application_path.replace("/", "\\"));
    }
  else
    {
      settings->remove(application_name);
    }
  delete settings;
}

void SingleApplication::toSlowly()
{
  if(!m_mainWindow) return;
  if(m_animation)delete m_animation;
  m_animation = new QPropertyAnimation(m_mainWindow, "windowOpacity");
  m_animation->setDuration(2000);
  m_animation->setStartValue(0);
  m_animation->setEndValue(1);
  m_animation->start();
}

void SingleApplication::toTray()
{
  if (!QSystemTrayIcon::isSystemTrayAvailable()) {
          QMessageBox::critical(NULL, QObject::tr("系统托盘"), QObject::tr("不支持系统托盘"));
          return;
      }

  if(!m_mainWindow) return;
  SystemTrayMx *systemTray = new SystemTrayMx(m_mainWindow);
  systemTray->setMiddleClickText(tr("系统托盘"));
  systemTray->setToolTips(tr("应用程序系统托盘"));
  systemTray->setHideText(tr("苟富贵勿相忘"));
  systemTray->setVisible(false);
  connect(m_mainWindow,SIGNAL(signalHide()),systemTray,SLOT(slotShow()));
}

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 通过socket通讯实现程序单实例运行，监听到新的连接时触发该函数
////////////////////////////////////////////////////////////////////////////////
void SingleApplication::newLocalConnection()
{
  QLocalSocket *socket = m_localServer->nextPendingConnection();
  if (!socket)
    return;
  socket->waitForReadyRead(1000);
  //QTextStream stream(socket);
  //其他处理
  delete socket;
  if (m_mainWindow != nullptr)
    {
      //激活窗口
      m_mainWindow->raise();
      m_mainWindow->activateWindow();
      m_mainWindow->setWindowState((m_mainWindow->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
      m_mainWindow->show();
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 通过socket通讯实现程序单实例运行，
// 初始化本地连接，如果连接不上server，则创建，否则退出
////////////////////////////////////////////////////////////////////////////////
void SingleApplication::initLocalConnection()
{
  m_bRunning = false;
  QLocalSocket socket;
  socket.connectToServer(m_serverName);
  if(socket.waitForConnected(500))
    {
      m_bRunning = true;
      // 其他处理，如：将启动参数发送到服务端
      QTextStream stream(&socket);
      QStringList args = QCoreApplication::arguments();
      if (args.count() > 1)
        stream << args.last();
      else
        stream << QString();
      stream.flush();
      socket.waitForBytesWritten();

      return;
    }

  //连接不上服务器，就创建一个
  newLocalServer();
}

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 创建LocalServer
////////////////////////////////////////////////////////////////////////////////
void SingleApplication::newLocalServer()
{
  m_localServer = new QLocalServer(this);
  connect(m_localServer, SIGNAL(newConnection()), this, SLOT(newLocalConnection()));
  if(!m_localServer->listen(m_serverName))
    {
      // 此时监听失败，可能是程序崩溃时,残留进程服务导致的,移除之
      if(m_localServer->serverError() == QAbstractSocket::AddressInUseError)
        {
          QLocalServer::removeServer(m_serverName); // <-- 重点
          m_localServer->listen(m_serverName); // 再次监听
        }
    }
}
