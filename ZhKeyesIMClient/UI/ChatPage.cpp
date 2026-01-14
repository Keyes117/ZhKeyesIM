#include "ChatPage.h"

#include <QPainter>
#include <QStyleOption>

#include "UI/ChatItemBase.h"
#include "UI/TextBubble.h"
#include "UI/MEssageTextEdit.h"
#include "UI/PictureBubble.h"

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.button_send->SetState("normal", "hover", "press");
    ui.label_emo->setState("normal", "hover", "press", "normal", "hover", "press");
    ui.label_file->setState("normal", "hover", "press", "normal", "hover", "press");

    connect(ui.button_send, &QPushButton::clicked, this, &ChatPage::onButtonSendClicked);
}

ChatPage::~ChatPage()
{}

void ChatPage::onButtonSendClicked()
{
    auto pTextEdit = ui.textEdit_chat;
    ChatRole role = ChatRole::Self;
    QString userName = QString::fromLocal8Bit("ÁµÁµ·ç³½");
    QString userIcon = ":/res/res/head_1.jpg";
    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    for (int i = 0; i < msgList.size(); ++i)
    {
        QString type = msgList[i].msgFlag;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));
        QWidget* pBubble = nullptr;
        if (type == "text")
        {
            pBubble = new TextBubble(role, msgList[i].content);
        }
        else if (type == "image")
        {
            pBubble = new PictureBubble(QPixmap(msgList[i].content), role);
        }
        else if (type == "file")
        {
        }
        if (pBubble != nullptr)
        {
            pChatItem->setWidget(pBubble);
            ui.widget_chatView->appendChatItem(pChatItem);
        }
    }
}

void ChatPage::paintEvent(QPaintEvent * event)
{
    QStyleOption opt;
    opt.initFrom(this);

    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

