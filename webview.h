///////////////////////////////////////////////////////
/// 版权所有：智慧中华技术有限公司，保留所有权利
/// @file
/// @brief
/// @author
/// @date
/// @version  1.0
/// @note
///////////////////////////////////////////////////////
#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWidget>
QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
class QLineEdit;
class WebView : public QWidget
{
  Q_OBJECT
public:
  explicit WebView(QWidget *parent = 0);
  ~WebView();
    QString setUrl(const QString& web, QString dir);
    void getHttpData(const QString &modelUrl, QByteArray &data);
//    void handleData(QByteArray &data);
signals:

public slots:
  void onQuery(QNetworkReply*);

//  void onClicked();
private:
  QNetworkAccessManager*  m_network;
  QLineEdit*              m_addrEdit;
  QLineEdit*              m_contentEdit;
  bool                    m_flag;
  QString*                m_content;

};
QT_END_NAMESPACE
#endif // WEBVIEW_H
