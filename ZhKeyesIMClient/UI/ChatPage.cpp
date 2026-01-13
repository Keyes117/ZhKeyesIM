#include "ChatPage.h"

#include <QPainter>
#include <QStyleOption>

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.button_send->SetState("normal", "hover", "press");
    ui.label_emo->setState("normal", "hover", "press", "normal", "hover", "press");
    ui.label_file->setState("normal", "hover", "press", "normal", "hover", "press");
}

ChatPage::~ChatPage()
{}

void ChatPage::paintEvent(QPaintEvent * event)
{
    QStyleOption opt;
    opt.initFrom(this);

    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}

