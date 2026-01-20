#include "ContactUserItem.h"

ContactUserItem::ContactUserItem(QWidget *parent)
    : ListItemBase(parent)
{
    ui.setupUi(this);

    setItemType(ListItemType::CONTACT_USER_ITEM);
    ui.label_redPoint->raise();
    showRedPoint(true);
}

ContactUserItem::~ContactUserItem()
{}

QSize ContactUserItem::sizeHint() const
{
    return QSize(250,70);
}

void ContactUserItem::setInfo(std::shared_ptr<AuthInfo> spAuthInfo)
{
    m_info = std::make_shared<UserInfo>(spAuthInfo);

    QPixmap pixmap(m_info->_icon);

    ui.label_icon->setPixmap(pixmap.scaled(ui.label_icon->size(), Qt::KeepAspectRatio,
        Qt::SmoothTransformation));

    ui.label_icon->setScaledContents(true);
    ui.label_username->setText(m_info->_name);
}


void ContactUserItem::setInfo(std::shared_ptr<AuthRsp>  spAuthRsp)
{
    m_info = std::make_shared<UserInfo>(spAuthRsp);

    QPixmap pixmap(m_info->_icon);

    ui.label_icon->setPixmap(pixmap.scaled(ui.label_icon->size(), Qt::KeepAspectRatio, 
        Qt::SmoothTransformation));
    ui.label_icon->setScaledContents(true);
    ui.label_username->setText(m_info->_name);
}

void ContactUserItem::setInfo(int uid, const QString& name, const QString& icon)
{
    m_info = std::make_shared<UserInfo>(uid, name, icon);

    QPixmap pixmap(m_info->_icon);

    ui.label_icon->setPixmap(pixmap.scaled(ui.label_icon->size(), Qt::KeepAspectRatio, 
        Qt::SmoothTransformation));
    ui.label_icon->setScaledContents(true);
    ui.label_username->setText(m_info->_name);
}

void ContactUserItem::showRedPoint(bool bShow)
{
    if (bShow) 
    {
        ui.label_redPoint->show();
    }
    else
    {
        ui.label_redPoint->hide();
    }
}
