#include "webview.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegExp>
#include <QLineEdit>
#include <QString>
#include <QCoreApplication>
#include <QDebug>
WebView::WebView(QWidget *parent) :
  QWidget(parent)
{
  m_network = new QNetworkAccessManager(this);
  connect(m_network,SIGNAL(finished(QNetworkReply*)),this, SLOT(onQuery(QNetworkReply*)));
  m_content = new QString("");
  m_contentEdit = new QLineEdit(this);
  m_addrEdit = new QLineEdit(this);
}

WebView::~WebView()
{
  if(m_network)delete m_network;
  if(m_contentEdit)delete m_contentEdit;
  if(m_addrEdit)delete m_addrEdit;
  if(m_content) delete m_content;
}

void WebView::setUrl(const QString &web)
{
  m_addrEdit->setText(web);
}

void WebView::onClicked()
{
  QString uri = m_addrEdit->text();
  for(int i=0; i<11; i++)
    {
      QString url2(uri);
      url2.append(QString::number(i,10));
      qDebug(url2.toLocal8Bit());
      m_network->get(QNetworkRequest(QUrl(url2)));
      while(!m_flag)
        QCoreApplication::processEvents();
      m_flag = false;
    }
  m_contentEdit->setText(*m_content);
}

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
  //qDebug(QString::number(pos).toAscii());
  m_flag = true;
}
