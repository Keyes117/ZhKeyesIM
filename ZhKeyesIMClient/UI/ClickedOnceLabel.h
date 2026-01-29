#ifndef ZHKEYESIMCLIENT_UI_CLICKEDONCELABEL_H_
#define ZHKEYESIMCLIENT_UI_CLICKEDONCELABEL_H_

#include <QLabel>


class ClickedOnceLabel : public QLabel
{
    Q_OBJECT
public:
    ClickedOnceLabel(QWidget *parent=nullptr);
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
signals:
    void clicked(QString);
};


#endif //!ZHKEYESIMCLIENT_UI_CLICKEDONCELABEL_H_