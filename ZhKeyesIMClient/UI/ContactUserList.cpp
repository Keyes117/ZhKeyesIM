#include "ContactUserList.h"

#include <QRandomGenerator>
#include <QScrollbar>

#include "Base/global.h"
#include "UI/GroupTipItem.h"

ContactUserListWidget::ContactUserListWidget(QWidget* parent)
    :QListWidget(parent)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->viewport()->installEventFilter(this);

    addContactUserList();

    connect(this, &QListWidget::itemClicked, this, &ContactUserListWidget::onItemClicked);


}

void ContactUserListWidget::showRedPoint(bool bShow)
{
    m_addFriendItem->showRedPoint();
}

bool ContactUserListWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched = this->viewport())
    {
        if (event->type() == QEvent::Enter)
        {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if (event->type() == QEvent::Leave)
        {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    if (watched == this->viewport() && event->type() == QEvent::Wheel)
    {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数
        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);
        // 检查是否滚动到底部
        QScrollBar* scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        //int pageSize = 10; // 每页加载的联系人数量
        if (maxScrollValue - currentValue <= 0)
        {
            // 滚动到底部，加载新的联系人
            //发送信号通知聊天界面加载更多聊天内容
            emit loadingContactUser();
        }
        return true; 
    }
    return QListWidget::eventFilter(watched, event);

}

void ContactUserListWidget::addContactUserList()
{
    auto* groupTip = new GroupTipItem();
    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(groupTip->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, groupTip);

    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

    m_addFriendItem = new ContactUserItem();
    m_addFriendItem->setInfo(0, tr("新的朋友"), ":/res/res/add_friend.png");
    m_addFriendItem->setItemType(ListItemType::APPLY_FRIEND_ITEM);

    QListWidgetItem* addItem = new QListWidgetItem();
    addItem->setSizeHint(m_addFriendItem->sizeHint());
    this->addItem(addItem);
    this->setItemWidget(addItem, m_addFriendItem);

    this->setCurrentItem(addItem);
    
    auto* groupContact = new GroupTipItem();
    groupContact->SetGroupTip(tr("联系人"));
    m_groupItem = new QListWidgetItem();
    m_groupItem->setSizeHint(groupContact->sizeHint());
    this->addItem(m_groupItem);
    this->setItemWidget(m_groupItem, groupContact);
    m_groupItem->setFlags(m_groupItem->flags() & ~Qt::ItemIsSelectable);

    // 创建QListWidgetItem，并设置自定义的widget
    for (int i = 0; i < 13; i++) {
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue % strs.size();
        int head_i = randomValue % heads.size();
        int name_i = randomValue % names.size();
        auto* contactUserItem = new ContactUserItem();
        contactUserItem->setInfo(0, names[name_i], heads[head_i]);
        QListWidgetItem* item = new QListWidgetItem;
        item->setSizeHint(contactUserItem->sizeHint());
        this->addItem(item);
        this->setItemWidget(item, contactUserItem);
    }
}


void ContactUserListWidget::onItemClicked(QListWidgetItem* item)
{
    QWidget* widget = this->itemWidget(item); // 获取自定义widget对象
    if (!widget) {
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if (!customItem) {
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->getItemType();
    if (itemType == ListItemType::INVALID_ITEM
        || itemType == ListItemType::GROUP_TIP_ITEM) {
        qDebug() << "slot invalid item clicked ";
        return;
    }

    if (itemType == ListItemType::APPLY_FRIEND_ITEM) {

        // 创建对话框，提示用户
        qDebug() << "apply friend item clicked ";
        //跳转到好友申请界面
        emit switchApplyFriendPage();
        return;
    }

    if (itemType == ListItemType::CONTACT_USER_ITEM) {
        // 创建对话框，提示用户
        qDebug() << "contact user item clicked ";

        auto con_item = qobject_cast<ContactUserItem*>(customItem);
        auto user_info = con_item->getInfo();
        //跳转到好友申请界面
        emit switchFriendInfoPage(user_info);
        return;
    }
}