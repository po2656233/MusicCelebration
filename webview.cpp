#include "webview.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegExp>
#include <QLineEdit>
#include <QString>
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QDir>
WebView::WebView(QWidget *parent) :
    QWidget(parent)
{
    m_network = new QNetworkAccessManager(this);
    connect(m_network,SIGNAL(finished(QNetworkReply*)),this, SLOT(onQuery(QNetworkReply*)));
    m_content = new QString("");
    //  m_contentEdit = new QLineEdit(this);
    //  m_addrEdit = new QLineEdit(this);
}

WebView::~WebView()
{
    //  if(m_network)delete m_network;
    //  if(m_contentEdit)delete m_contentEdit;
    //  if(m_addrEdit)delete m_addrEdit;
    //  if(m_content) delete m_content;
}

//void WebView::setUrl(const QString &web)
//{
//    QString uri = web;
//    QByteArray  data;
//    getHttpData(web,data);
//    qDebug()<<"获取到的网络数据"<<data;
//    for(int i=0; i<11; i++)
//      {
//        QString url2(uri);
//        url2.append(QString::number(i,10));
//        qDebug(url2.toLocal8Bit());
//        m_network->get(QNetworkRequest(QUrl(url2)));
//        while(!m_flag)
//          QCoreApplication::processEvents();
//        m_flag = false;
//      }
//}
QString WebView::setUrl(const QString &web,QString dir)
{
    //判断当前 Qt 是否支持 OpenSSL. 若输出：OpenSSL支持情况: false，那说明当前 Qt 不支持 OpenSSL
    qDebug() <<"OpenSSL支持的版本:"<<QSslSocket::sslLibraryBuildVersionString()<< "OpenSSL支持情况:" << QSslSocket::supportsSsl();
    QUrl url(web);
    QEventLoop loop;
    QNetworkAccessManager manager;

    //发出请求
    QNetworkReply *reply=manager.get(QNetworkRequest(url));

    //请求结束并下载完成后，退出子事件循环
    connect(reply,SIGNAL(finished()),&loop,SLOT(quit()),Qt::UniqueConnection);

    //开启子事件循环
    loop.exec();

    //将读取到的信息写入文件
    QByteArray sourceCode=reply->readAll();
    reply->deleteLater();
    if(sourceCode.size()>10){
//        QMessageBox::information(this,tr("提示"),web+"网站数据获取成功！",QMessageBox::Ok);
    }
    else{
//        QMessageBox::information(this,tr("提示"),web+"网站数据获取失败！",QMessageBox::Ok);
        return "";
    }

    // 保存文件
    QString fileName =QDir::toNativeSeparators(dir+"\\"+url.fileName());
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.write(sourceCode);
    file.close();
    if(file.size()<10){
        file.remove();
    }

    qDebug()<<"当前请求的网络地址"<<web<<" 保存为:"<<fileName;
    return fileName;
}
void WebView::getHttpData(const QString & modelUrl,QByteArray & data)
{
    QUrl url = QUrl::fromUserInput(modelUrl.toUtf8());
    QNetworkReply * reply_ =  m_network->get(QNetworkRequest(url));
    connect(reply_, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),[&](QNetworkReply::NetworkError code)
    {
        switch(static_cast<int>(code))
        {
        case QNetworkReply::ConnectionRefusedError:
            qDebug() << "远程服务器拒绝连接（服务器不接受请求）";
            break;
        case QNetworkReply::HostNotFoundError:
            qDebug() << "找不到远程主机名（无效的主机名）";
            break;
        case QNetworkReply::TimeoutError:
            qDebug() << "与远程服务器的连接超时";
            break;
        default:
            break;
        }
    });

    QEventLoop eventLoop2;
    connect(reply_, &QNetworkReply::finished, &eventLoop2, &QEventLoop::quit);
    eventLoop2.exec(QEventLoop::ExcludeUserInputEvents);

    data = reply_->readAll();
    reply_->deleteLater();
}

//void WebView::handleData(QByteArray & data)
//{
//    QString str = QString(data);
//    qDebug()<<endl<<endl<<endl<<str;

//    QRegExp exp("http://pic.k73.com/up/soft/.+\.(png|jpg)");
//    exp.setMinimal(false);

//    QStringList list = str.split("\n");
//    urlList.clear();

//    for (int i = 0;i < list.size();++i)
//    {
//        int pos = 0;
//        while ((pos = exp.indexIn(list.at(i) , pos)) != -1)
//        {
//            pos += exp.matchedLength();
//            qDebug() << "pos :" << pos;
//            qDebug() << exp.cap(0);
//            urlList << exp.cap(0);
//        }
//    }
//}

//void WebView::onClicked()
//{
//    QRegExp exp("[^/]+\.(png|jpg)$");
//    exp.setMinimal(false);

//    int handleNumber = 0;
//    foreach (auto url, urlList)
//    {
//        int pos = 0;
//        QString fileName;
//        while ((pos = exp.indexIn(url , pos)) != -1)
//        {
//            pos += exp.matchedLength();
//            fileName = exp.cap(0);
//        }

//        QByteArray data;
//        getHttpData(url,data);
//        QPixmap pixmap;
//        pixmap.loadFromData(data);
//        pixmap.save(fileName);

//        ++handleNumber;
//    }
//}

void WebView::onQuery(QNetworkReply * reply)
{
    QString buffer = reply->readAll();
    QRegExp exp("<div class=\"content\" title=\"(.*)\">(.*)</div>");
    exp.setMinimal(true);
    int pos=0;
    while ((pos = exp.indexIn(buffer,pos)) != -1) {
        m_content->append(exp.cap(1));
        m_content->append(exp.cap(2));
        pos += exp.matchedLength();
        //qDebug(QString::number(pos).toAscii());
    }
    qDebug()<<(QString::number(pos));
    m_flag = true;
}
