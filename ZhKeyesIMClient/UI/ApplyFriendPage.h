#pragma once

#include <memory>

#include <QWidget>
#include "ui_ApplyFriendPage.h"

#include "Base/UserData.h"
#include "UI/ApplyFriendItem.h"

class ApplyFriendPage : public QWidget
{
    Q_OBJECT

public:
    ApplyFriendPage(QWidget *parent = nullptr);
    ~ApplyFriendPage();


    void addNewApply(std::shared_ptr<AddFriendApply> apply);

protected:
    void paintEvent(QPaintEvent* event);

private:
    void loadApplyList();

public slots:
    void onAuthResponse(std::shared_ptr<AuthRsp> spRsp);

signals:
    void showSearch(bool);
private:
    Ui::ApplyFriendPageClass ui;

    std::unordered_map<int, ApplyFriendItem*> m_unAuthItems;


};

