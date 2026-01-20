#include "ApplyFriendPage.h"

ApplyFriendPage::ApplyFriendPage(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    //connect(ui.listWidget_applyFriend,)
}

ApplyFriendPage::~ApplyFriendPage()
{}

void ApplyFriendPage::addNewApply(std::shared_ptr<AddFriendApply> apply)
{
}

void ApplyFriendPage::paintEvent(QPaintEvent* event)
{
}

void ApplyFriendPage::loadApplyList()
{
}

void ApplyFriendPage::onAuthResponse(std::shared_ptr<AuthRsp> spRsp);

