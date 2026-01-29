#include "FriendLabelFrame.h"

#include <QFontMetrics>

FriendLabelFrame::FriendLabelFrame(QWidget *parent)
    : QFrame(parent)
{
    ui.setupUi(this);


    ui.label__close->setState("normal", "hover", "pressed",
        "selected_normal", "selected_hover", "selected_pressed");

    connect(ui.label__close, &ClickedLabel::clicked, this, &FriendLabelFrame::onLabelCloseClicked);
}

FriendLabelFrame::~FriendLabelFrame()
{}

void FriendLabelFrame::SetText(QString text)
{
    m_text = text;
    ui.label_tip->setText(m_text);
    ui.label_tip->adjustSize();

    QFontMetrics fontMetrics(ui.label_tip->font());
    auto textWidth = fontMetrics.horizontalAdvance(ui.label_tip->text());
    auto textHeight = fontMetrics.height();

    this->setFixedWidth(ui.label_tip->width() + ui.label__close->width() + 5);
    this->setFixedHeight(textHeight + 2);

    m_width = this->width();
    m_height = this->height();


}

int FriendLabelFrame::Width()
{
    return m_width;
}

int FriendLabelFrame::Height()
{
    return m_height;
}

QString FriendLabelFrame::Text()
{
    return m_text;
}

void FriendLabelFrame::onLabelCloseClicked()
{
    emit close(m_text);
}

