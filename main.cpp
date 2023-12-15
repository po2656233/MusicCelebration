///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file
/// @brief
/// @author
/// @date
/// @version  1.0
/// @note
/// 1、歌词未完成
/// 2、快进未完成*
/// 3、禁音未完成*
/// 4、用户配置信息未完成
/// 5、数据库线程
/// 6、用户日志信息
/// 7、添加娱乐项目
///////////////////////////////////////////////////////
#include "musicshow.h"
#include "singleapplication.h"
#include <QIcon>
#include <QFileIconProvider>


int main(int argc, char *argv[])
{
    SingleApplication a(argc, argv);
    if (!a.isRunning())
    {
        a.setOrganizationName(QObject::tr("CZH"));
        a.setOrganizationDomain(QObject::tr("www.baidu.com"));
        a.setApplicationName(QObject::tr("音乐庆典"));
        a.setApplicationVersion(QObject::tr("1.0"));
        a.setWindowIcon(QIcon(":/img/tone.ico"));
        a.setAutoStart(true);//开机启动

        //加载样式表
        QFile qss(":./uiDesigner.qss");
        if (qss.open(QFile::ReadOnly))
            qApp->setStyleSheet(qss.readAll());
        qss.close();

        //传入一个要激活程序的窗口，当多开时会激活已有进程的窗口，且多开失败
        MusicShow w;

        a.setMainWindow(&w);
        a.toSlowly();
        a.toTray();
        w.show();
        return a.exec();
    }
    return 0;
}
