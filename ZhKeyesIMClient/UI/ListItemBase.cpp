#include "ListItemBase.h"

#include <QPainter>
#include <QStyleOption>

ListItemBase::ListItemBase(QWidget* parent) :
    QWidget(parent)
{
}

void ListItemBase::setItemType(ListItemType itemType)
{
    m_itemType = itemType;
}

ListItemType ListItemBase::GetItemType()
{
    return m_itemType;
}

void ListItemBase::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);

    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
