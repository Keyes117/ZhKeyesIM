#ifndef ZHKEYESIMCLIENT_UI_CHATUSERWIDGET_H_
#define ZHKEYESIMCLIENT_UI_CHATUSERWIDGET_H_
#include <QWidget>

#include "ui_ChatUseritem.h"

#include "ListItemBase.h"

class ChatUserItem : public ListItemBase
{
    Q_OBJECT
public:
    explicit ChatUserItem(QWidget* parent = nullptr);
    ~ChatUserItem();

    virtual QSize sizeHint() const override {
        return QSize(250, 70);
    }

    void setInfo(const QString& name, const QString& head,const QString& msg);



private:
    Ui::ChatUserItemClass m_ui;

    QString m_name;
    QString m_head;
    QString m_msg;
};

#endif