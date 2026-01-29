#include "ApplyFriendItem.h"

#include "UI/ClickedButton.h"

ApplyFriendItem::ApplyFriendItem(QWidget *parent)
    : ListItemBase(parent)
{
    ui.setupUi(this);

    setItemType(ListItemType::APPLY_FRIEND_ITEM);
    ui.button_add->SetState("normal", "hover", "press");
    ui.button_add->hide();

    connect(ui.button_add, &ClickedButton::clicked, [this]() {
        emit this->authFriend(m_applyInfo);
        });
}

ApplyFriendItem::~ApplyFriendItem()
{}

void ApplyFriendItem::setInfo(std::shared_ptr<ApplyInfo> spApplyInfo)
{
    m_applyInfo = spApplyInfo;

    QPixmap pixmap(m_applyInfo->_icon);
    // 设置图片自动缩放
    ui.label_icon->setPixmap(pixmap.scaled(ui.label_icon->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui.label_icon->setScaledContents(true);
    ui.label_username->setText(m_applyInfo->_name);
    ui.label_userchat->setText(m_applyInfo->_desc);
}

void ApplyFriendItem::showButtonAdd(bool bShow)
{
    if (bShow) {
        ui.button_add->show();
        ui.label_alreadyAdd->hide();
        m_added = false;
    }
    else {
        ui.button_add->hide();
        ui.label_alreadyAdd->show();
        m_added = true;
    }
}

