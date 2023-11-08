#include "systemtraymx.h"
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QAction>
#include <QUrl>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QDesktopServices>
#include <QApplication>

SystemTrayMx::SystemTrayMx(QWidget *parent)
    : QSystemTrayIcon(parent)
    , middleText("")
    , toolTils("")
    , hideText("")
{
    parentWidget = parent;
    createActions();
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(slotActivated(QSystemTrayIcon::ActivationReason)));
}

SystemTrayMx::~SystemTrayMx()
{

}

void SystemTrayMx::setParentWidget(QWidget *parent)
{
    parentWidget = parent;
}

void SystemTrayMx::setMiddleClickText(const QString &tips)
{
    middleText = tips;
}

void SystemTrayMx::setHideText(const QString &text)
{
    hideText = text;
}

void SystemTrayMx::setToolTips(const QString &tips)
{
    this->setToolTip(tips);
}

void SystemTrayMx::startHideTips()
{
    this->showMessage(tr("提示信息："), hideText, QSystemTrayIcon::Information, 2000);
}

void SystemTrayMx::createActions()
{
    this->setIcon(QIcon(":/img/tone.ico"));
    trayIconMenu = new QMenu();
    createTopWidget();

    funMenu = new QMenu();
    showAction = new QAction(this);
    helpAction = new QAction(this);
    quitAction = new QAction(this);

    funMenu->setIcon(QIcon(":/img/home.ico"));
    showAction->setIcon(QIcon(":/img/show.ico"));
    helpAction->setIcon(QIcon(":/img/help.ico"));
    quitAction->setIcon(QIcon(":/img/quit.ico"));

    funMenu->setTitle(tr("功能"));
    showAction->setText(tr("显示"));
    helpAction->setText(tr("帮助"));
    quitAction->setText(tr("退出"));

    trayIconMenu->addAction(topWidgetAction);
    //trayIconMenu->setFixedWidth(250);
    trayIconMenu->addAction(showAction);
    funMenu->addAction(helpAction);
    trayIconMenu->addMenu(funMenu);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    this->setContextMenu(trayIconMenu);
    connect(showAction, SIGNAL(triggered()), this, SLOT(showParentWidget()));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(slotHelpAction()));
    connect(quitAction, &QAction::triggered, this, [=](){qApp->quit();});
}

void SystemTrayMx::createTopWidget()
{
    topWidget = new QWidget();
    topWidget->setStyleSheet("QWidget{background:#2FC77B;color:#000000}QPushButton:hover{color:#FFFFFF}QPushButton:disabled{color:#666666;border-style:flat;}");
    topWidgetAction = new QWidgetAction(trayIconMenu);
    QLabel *titleLabel = new QLabel();
    titleLabel->setText(tr("系统托盘显示"));
/*
    showButton = new QPushButton();
    showButton->setText(tr("显示"));
    showButton->setFocusPolicy(Qt::NoFocus);

    hideButton = new QPushButton();
    hideButton->setText(tr("隐藏"));
    hideButton->setFocusPolicy(Qt::NoFocus);
    showButton->setCursor(Qt::PointingHandCursor);
    hideButton->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(showButton);
    hLayout->addStretch();
    hLayout->addWidget(hideButton);
    hLayout->setSpacing(10);
    hLayout->setContentsMargins(0, 0, 0, 0);
*/
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(titleLabel);
   // vLayout->addLayout(hLayout);
    vLayout->setSpacing(10);
    vLayout->setContentsMargins(10, 5, 10, 5);
    topWidget->setLayout(vLayout);
    topWidgetAction->setDefaultWidget(topWidget);
    //connect(showButton, SIGNAL(clicked()), this, SLOT(slotClicked()));
    //connect(hideButton, SIGNAL(clicked()), this, SLOT(slotClicked()));
}

void SystemTrayMx::slotActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (parentWidget) {
        switch (reason) {
            case QSystemTrayIcon::Trigger:
            case QSystemTrayIcon::DoubleClick: {
                if (parentWidget->isHidden() || parentWidget->windowState() == Qt::WindowMinimized) {
                    //showNormal(),showMinimized(),showMaxmized(),showFullScreen()
                    parentWidget->showNormal();
                    parentWidget->raise();
                    parentWidget->activateWindow();
                }
                else {
                    parentWidget->hide();
                    startHideTips();
                }
                break;
            }
            case QSystemTrayIcon::MiddleClick: {
                if (!middleText.isEmpty()) {
                    this->showMessage(tr("提示信息："), middleText, QSystemTrayIcon::Information, 3000);
                }
                break;
            }
            case QSystemTrayIcon::Context: {
                if (parentWidget->isHidden() || parentWidget->windowState() == Qt::WindowMinimized) {
                    showAction->setText(tr("显示"));
                    //showButton->setEnabled(true);
                    //hideButton->setEnabled(false);
                }
                else {
                    showAction->setText(tr("隐藏"));
                    //showButton->setEnabled(false);
                    //hideButton->setEnabled(true);
                }
                break;
            }
            default:
                break;
        }
    }
}

void SystemTrayMx::showParentWidget()
{
  if(!parentWidget||!showAction) return;
    if (showAction->text() == tr("显示")) {
        parentWidget->showNormal();
        parentWidget->raise();
        parentWidget->activateWindow();
    }
    else {
        parentWidget->hide();
        startHideTips();
    }
}

void SystemTrayMx::slotHelpAction()
{
    const QUrl AuthorUrl("https://user.qzone.qq.com/1047928853/");
    //qDebug() << AuthorUrl.scheme();
    //qDebug() << AuthorUrl.port();
    QDesktopServices::openUrl(AuthorUrl);
}

void SystemTrayMx::slotClicked()
{
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (button == showButton) {
        parentWidget->showNormal();
        parentWidget->raise();
        parentWidget->activateWindow();
    }
    else {
        parentWidget->hide();
        startHideTips();
    }
}
