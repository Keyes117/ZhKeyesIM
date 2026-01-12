#ifndef UI_CHATUSERLIST_H_
#define UI_CHATUSERLIST_H_


#include <QListWidget>
#include <QWheelEvent>
#include <QEvent>
#include <QScrollBar>
#include <QDebug>
#include <qcoreevent.h>

class ChatUserList : public QListWidget
{
    Q_OBJECT
public:
    ChatUserList(QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

signals:
    void loadingChatUser();
};

#endif