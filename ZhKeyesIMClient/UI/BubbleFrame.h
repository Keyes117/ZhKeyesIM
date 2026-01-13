#ifndef ZHKEYESIMCLIENT_UI_BUBBLEFRAME_H_
#define ZHKEYESIMCLIENT_UI_BUBBLEFRAME_H_


#include <QFrame>
#include "UI/global.h"

class QHBoxLayout;

class BubbleFrame : public QFrame
{
    Q_OBJECT
public:
    BubbleFrame(ChatRole role, QWidget* parent = nullptr);
    void setMargin(int margin);

    void setWidget(QWidget* widget);

protected:
    void paintEvent(QPaintEvent* event);


private:
    QHBoxLayout* m_pHLayout;
    ChatRole m_role;
    int     m_margin = 3;
};

#endif