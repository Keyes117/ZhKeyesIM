#include "ChatUserWidget.h"

#include <QPixmap>

ChatUserWidget::ChatUserWidget(QWidget *parent)
    : ListItemBase(parent)
{ 
    m_ui.setupUi(this);
    setItemType(ListItemType::CHAT_USER_ITEM);
}

ChatUserWidget::~ChatUserWidget()
{}

void ChatUserWidget::setInfo(const QString& name, const QString& head, const QString& msg)
{
    m_name =    name;
    m_head =    head;
    m_msg  =     msg;


    // 加载图片
    QPixmap pixmap(m_head);
    // 设置图片自动缩放
  
    m_ui.label_icon->setPixmap(pixmap.scaled(m_ui.label_icon->size(),
        Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_ui.label_icon->setScaledContents(true);
    m_ui.label_name->setText(m_name);
    m_ui.label_msg->setText(m_msg);
    

}

