#ifndef ZHKEYESIMCLIENT_UI_LISTITEMBASE_H_H
#define ZHKEYESIMCLIENT_UI_LISTITEMBASE_H_H

#include <QWidget>
#include <QPaintEvent>

#include "Base/global.h"

class ListItemBase : public QWidget
{
    Q_OBJECT
public:
    explicit ListItemBase(QWidget* parent = nullptr);
    virtual ~ListItemBase() = default;
    
    virtual void setItemType(ListItemType itemType);

    virtual ListItemType getItemType();

protected:
    virtual void paintEvent(QPaintEvent* event);
private:
    ListItemType m_itemType;

    
};


#endif