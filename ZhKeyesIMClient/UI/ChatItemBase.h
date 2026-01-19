#ifndef ZHKEYESIMCLIENT_UI_CHATITEMBASE_H_
#define ZHKEYESIMCLIENT_UI_CHATITEMBASE_H_

#include <QWidget>

#include "Base/global.h"

class QLabel;
class BubbleFrame;

//自己发出的消息
/*
* -------------------------------        
* |       |NameLable | IconLabel |
* -------------------------------
* |Spacer | Bubble   |           |
* -------------------------------
*/

/** 别人发的消息
 ** -----------------------------      
* |IconLabel |NameLable |        |
* -------------------------------
* |          | Bubble   | Spacer |
* -------------------------------
 */

class ChatItemBase : public QWidget
{
    Q_OBJECT
public:
    explicit ChatItemBase(ChatRole role, QWidget* parent = nullptr);
    virtual ~ChatItemBase() = default;

    void setUserName(const QString& name);
    void setUserIcon(const QPixmap& icon);
    void setWidget(QWidget* widget);

private:
    ChatRole    m_role;
    QLabel* m_pNameLabel;
    QLabel* m_pIconLabel;
    QWidget* m_pBubble;
};


#endif