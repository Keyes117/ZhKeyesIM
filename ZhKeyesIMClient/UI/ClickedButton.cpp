#include "ClickedButton.h"

#include "UI/global.h"

ClickedButton::ClickedButton(QWidget* parent):
    QPushButton(parent)
{
    setCursor(Qt::PointingHandCursor);
}

ClickedButton::~ClickedButton()
{
}

void ClickedButton::SetState(QString normal, QString hover, QString press)
{
    m_hover = hover;
    m_normal = normal;
    m_press = press;

    setProperty("state", normal);
    repolish(this);
    update();
}

void ClickedButton::enterEvent(QEnterEvent* event)
{
    setProperty("state", m_hover);
    repolish(this);
    update();
    QPushButton::enterEvent(event);
}

void ClickedButton::leaveEvent(QEvent* event)
{
    setProperty("state", m_normal);
    repolish(this);
    update();
    QPushButton::leaveEvent(event);
}

void ClickedButton::mousePressEvent(QMouseEvent* event)
{
    setProperty("state", m_press);
    repolish(this);
    update();
    QPushButton::mousePressEvent(event);
}

void ClickedButton::mouseReleaseEvent(QMouseEvent* event)
{
    setProperty("state", m_hover);
    repolish(this);
    update();
    QPushButton::mouseReleaseEvent(event);
}
