#ifndef LINEEDITMX_H
#define LINEEDITMX_H

#include <QLineEdit>
class LineEditMx : public QLineEdit
{
  Q_OBJECT
public:
  explicit LineEditMx(QWidget *parent = 0);
  ~LineEditMx();
signals:
  void over();
protected:
     void keyPressEvent(QKeyEvent *event);
     void focusOutEvent(QFocusEvent *e);
};
#endif // LINEEDITMX_H
