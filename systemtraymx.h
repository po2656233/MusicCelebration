///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file
/// @brief
/// @author
/// @date
/// @version  1.0
/// @note
///////////////////////////////////////////////////////
#ifndef SYSTEMTRAYMX_H
#define SYSTEMTRAYMX_H

#include <QSystemTrayIcon>
QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
class QWidgetAction;
class QPushButton;
QT_END_NAMESPACE

class SystemTrayMx : public QSystemTrayIcon
{
  Q_OBJECT
public:
  explicit SystemTrayMx(QWidget *parent = 0);
  ~SystemTrayMx();
     void setParentWidget(QWidget *parent);
     void setMiddleClickText(const QString &tips);
     void setHideText(const QString &text);
     void setToolTips(const QString &tips);
     void startHideTips();

 private:
     void createActions();
     void createTopWidget();

 private slots:
     void slotActivated(QSystemTrayIcon::ActivationReason reason);
     void showParentWidget();
     void slotHelpAction();
     void slotClicked();

 private:
     QMenu *trayIconMenu;
     QAction *showAction;
     QAction *helpAction;
     QAction *quitAction;
     QWidget *parentWidget;
     QMenu *funMenu;
     QWidget *topWidget;
     QWidgetAction *topWidgetAction;
     QPushButton *showButton;
     QPushButton *hideButton;
     QString middleText;
     QString toolTils;
     QString hideText;
 };

#endif // SYSTEMTRAYMX_H
