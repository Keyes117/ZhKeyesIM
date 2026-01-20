#pragma once

#include <memory>

#include <UI/ListItemBase.h>
#include "ui_ApplyFriendItem.h"

#include "Base/UserData.h"

class ApplyFriendItem : public ListItemBase
{
    Q_OBJECT

public:
    ApplyFriendItem(QWidget *parent = nullptr);
    ~ApplyFriendItem();


    void setInfo(std::shared_ptr<ApplyInfo> spApplyInfo);
    void showButtonAdd(bool bShow);
    virtual QSize sizeHint() const override
    {
        return QSize(250, 80);
    }

    int GetUid() {
        return m_applyInfo->_uid;
    }

signals:
    void authFriend(std::shared_ptr<ApplyInfo> spApplyInfo);

private:
    Ui::ApplyFriendItem ui;

    std::shared_ptr<ApplyInfo> m_applyInfo;
    bool m_added;
};

