#ifndef ZHKEYESIMCLIENT_UI_CHATPAGE_H_
#define ZHKEYESIMCLIENT_UI_CHATPAGE_H_

#include <QWidget>
#include <QPaintEvent>

#include "ui_ChatPage.h"

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    ChatPage(QWidget *parent = nullptr);
    ~ChatPage();

protected:
    virtual void paintEvent(QPaintEvent* event);

private:
    Ui::ChatPageClass ui;
};


#endif  