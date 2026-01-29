#include "ApplyFriendPage.h"

#include <QRandomGenerator>
#include <QPainter>

#include "Base/UserSession.h"

ApplyFriendPage::ApplyFriendPage(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    //connect(ui.listWidget_applyFriend,&Apply)
    connect(ui.listWidget_applyFriend, &ApplyFriendListWidget::showSearch, this, &ApplyFriendPage::showSearch);
    loadApplyList();
    //接受TCP传递的authresp
}

ApplyFriendPage::~ApplyFriendPage()
{}

void ApplyFriendPage::addNewApply(std::shared_ptr<AddFriendApply> apply)
{
    int randomValue = QRandomGenerator::global()->bounded(100);

    int headIndex = randomValue % heads.size();
    auto* applyItem = new ApplyFriendItem();
    auto applyInfo = std::make_shared<ApplyInfo>(apply->_from_uid,
        apply->_name, apply->_desc, heads[headIndex], apply->_name, 0, 0);
    applyItem->setInfo(applyInfo);

    QListWidgetItem* item = new QListWidgetItem;

    item->setSizeHint(applyItem->sizeHint());
    item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
    ui.listWidget_applyFriend->insertItem(0, item);
    ui.listWidget_applyFriend->setItemWidget(item, applyItem);
    applyItem->showButtonAdd(true);

    connect(applyItem, &ApplyFriendItem::authFriend, [this](std::shared_ptr<ApplyInfo> applyInfo) {
        
            
        });
}

void ApplyFriendPage::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ApplyFriendPage::loadApplyList()
{
    //添加好友申请
    auto apply_list = UserSession::getInstance().GetApplyList();
    for (auto& apply : apply_list) {
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int head_i = randomValue % heads.size();
        auto* apply_item = new ApplyFriendItem();
        apply->SetIcon(heads[head_i]);
        apply_item->setInfo(apply);
        QListWidgetItem* item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(apply_item->sizeHint());
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        ui.listWidget_applyFriend->insertItem(0, item);
        ui.listWidget_applyFriend->setItemWidget(item, apply_item);
        if (apply->_status) {
            apply_item->showButtonAdd(false);
        }
        else {
            apply_item->showButtonAdd(true);
            auto uid = apply_item->GetUid();
            m_unAuthItems[uid] = apply_item;
        }
        //收到审核好友信号
        connect(apply_item, &ApplyFriendItem::authFriend, [this](std::shared_ptr<ApplyInfo> apply_info) {
            //            auto* authFriend = new AuthenFriend(this);
            //            authFriend->setModal(true);
            //            authFriend->SetApplyInfo(apply_info);
            //            authFriend->show();
            });
    }
    // 模拟假数据，创建QListWidgetItem，并设置自定义的widget
    for (int i = 0; i < 13; i++) {
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue % strs.size();
        int head_i = randomValue % heads.size();
        int name_i = randomValue % names.size();
        auto* apply_item = new ApplyFriendItem();
        auto apply = std::make_shared<ApplyInfo>(0, names[name_i], strs[str_i],
            heads[head_i], names[name_i], 0, 1);
        apply_item->setInfo(apply);
        QListWidgetItem* item = new QListWidgetItem;

        item->setSizeHint(apply_item->sizeHint());
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled & ~Qt::ItemIsSelectable);
        ui.listWidget_applyFriend->addItem(item);
        ui.listWidget_applyFriend->setItemWidget(item, apply_item);
        //收到审核好友信号
        connect(apply_item, &ApplyFriendItem::authFriend, [this](std::shared_ptr<ApplyInfo> apply_info) {
            //            auto *authFriend =  new AuthenFriend(this);
            //            authFriend->setModal(true);
            //            authFriend->SetApplyInfo(apply_info);
            //            authFriend->show();
            });
    }
}

void ApplyFriendPage::onAuthResponse(std::shared_ptr<AuthRsp> spRsp)
{
    auto uid = spRsp->_uid;
    auto find_iter = m_unAuthItems.find(uid);
    if (find_iter == m_unAuthItems.end()) {
        return;
    }
    find_iter->second->showButtonAdd(false);
}

