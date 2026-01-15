#ifndef ZHKEYESIMCLIENT_UI_ADDUSERITEM_H_
#define ZHKEYESIMCLIENT_UI_ADDUSERITEM_H_

#include "ListItemBase.h"
#include "ui_AddUserItem.h"

class AddUserItem : public ListItemBase
{
    Q_OBJECT

public:
    AddUserItem(QWidget *parent = nullptr);
    ~AddUserItem();

    QSize sizeHint() const override {
        return QSize(250, 70);
    }

private:
    Ui::AddUserItemClass ui;
};

#endif