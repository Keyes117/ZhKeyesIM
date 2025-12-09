#include "ClickedLabel.h"

ClickedLabel::ClickedLabel(QWidget* parent):
    QLabel(parent),
    m_curState(ClickState::Normal)
{
}

void ClickedLabel::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (m_curState == ClickState::Normal)
        {
            m_curState = ClickState::Selected;
            setProperty("state", m_selected_hover);
            repolish(this);
            update();
        }
        else
        {
            m_curState = ClickState::Normal;
            setProperty("state", m_normal_hover);
            repolish(this);
            update();
        }

        emit clicked();
    }

    QLabel::mousePressEvent(event);
}

void ClickedLabel::enterEvent(QEnterEvent* event)
{
    if (m_curState == ClickState::Normal)
    {
        setProperty("state", m_normal_hover);
        repolish(this);
        update();
    }
    else
    {
        setProperty("state", m_selected_hover);
        repolish(this);
        update();
    }

    QLabel::enterEvent(event);
}

void ClickedLabel::leaveEvent(QEvent* event)
{
    if (m_curState == ClickState::Normal)
    {
        setProperty("state", m_normal);
        repolish(this);
        update();
    }
    else
    {
        setProperty("state", m_selected);
        repolish(this);
        update();
    }

    QLabel::leaveEvent(event);
}

void ClickedLabel::setState(const QString& normal, const QString& hover, 
    const QString& press, const QString& select, 
    const QString& select_hover, const QString& select_press)
{

    m_normal = normal;
    m_normal_hover = hover;
    m_normal_press = press;
    m_selected = select;
    m_selected_hover = select_hover;
    m_selected_press = select_press;
    setProperty("state", normal);
    repolish(this);
}
