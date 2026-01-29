#include "AddUserItem.h"

AddUserItem::AddUserItem(QWidget *parent)
    : ListItemBase(parent)
{
    ui.setupUi(this);
    setItemType(ListItemType::ADD_USER_TIP_ITEM);
}

AddUserItem::~AddUserItem()
{}

