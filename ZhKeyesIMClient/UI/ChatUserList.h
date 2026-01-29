#ifndef ZHKEYESIMCLIENT_UI_CHATUSERLIST_H_
#define ZHKEYESIMCLIENT_UI_CHATUSERLIST_H_


#include <QListWidget>
#include <QWheelEvent>
#include <QEvent>
#include <QScrollBar>
#include <QDebug>
#include <qcoreevent.h>

class ChatUserListWidget : public QListWidget
{
    Q_OBJECT
public:
    ChatUserListWidget(QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

signals:
    void loadingChatUser();
};

#endif