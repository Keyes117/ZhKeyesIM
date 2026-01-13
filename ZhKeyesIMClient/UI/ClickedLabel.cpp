#include "ClickedLabel.h"

ClickedLabel::ClickedLabel(QWidget* parent):
    QLabel(parent),
    m_curState(ClickLbState::Normal)
{
}

void ClickedLabel::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (m_curState == ClickLbState::Normal)
        {
            m_curState = ClickLbState::Selected;
            setProperty("state", m_selected_press);
            repolish(this);
            update();
        }
        else
        {
            m_curState = ClickLbState::Normal;
            setProperty("state", m_normal_press);
            repolish(this);
            update();
        }

        emit clicked();
    }

    QLabel::mousePressEvent(event);
}

void ClickedLabel::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (m_curState == ClickLbState::Normal)
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
        emit clicked();
        return;
    }

    QLabel::mousePressEvent(event);
}

void ClickedLabel::enterEvent(QEnterEvent* event)
{
    if (m_curState == ClickLbState::Normal)
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
    if (m_curState == ClickLbState::Normal)
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
