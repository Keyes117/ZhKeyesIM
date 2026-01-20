#pragma once

#include <QSize>
#include <QWidget>

#include "ui_ContactUserItem.h"

#include "Base/UserData.h"
#include "UI/ListItemBase.h"


class ContactUserItem : public ListItemBase
{
    Q_OBJECT

public:
    ContactUserItem(QWidget *parent = nullptr);
    ~ContactUserItem();

    virtual QSize sizeHint() const override;
    void setInfo(std::shared_ptr<AuthInfo> spAuthInfo);
    void setInfo(std::shared_ptr<AuthRsp>  spAuthRsp);
    void setInfo(int uid, const QString& name, const QString& icon);
    void showRedPoint(bool bShow = false);


    std::shared_ptr<UserInfo> getInfo() { return m_info; }
private:
    Ui::ContactUserItemClass ui;

    std::shared_ptr<UserInfo> m_info;
};

