#include "StateWidget.h"

#include <QLabel>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>

StateWidget::StateWidget(QWidget* parent):
    QLabel(parent)
{
    setCursor(Qt::PointingHandCursor);

    addRedPoint();
}

void StateWidget::setState(const QString& normal, const QString& hover, 
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

ClickLbState StateWidget::getCurState()
{
    return m_curState;
}

void StateWidget::clearState()
{
    m_curState = ClickLbState::Normal;

    setProperty("state", m_normal);
    repolish(this);
    update();
}

void StateWidget::setSelected(bool bSelected)
{
    if (bSelected)
    {
        m_curState = ClickLbState::Selected;
        setProperty("state", m_selected);
        repolish(this);
        update();

    }
    else
    {
        m_curState = ClickLbState::Normal;
        setProperty("state", m_normal);
        repolish(this);
        update();
        return;
    }

    return;
}

void StateWidget::addRedPoint()
{
    //添加红点示意图
    m_red_point = new QLabel();
    m_red_point->setObjectName("red_point");
    QVBoxLayout* layout2 = new QVBoxLayout;
    m_red_point->setAlignment(Qt::AlignCenter);
    layout2->addWidget(m_red_point);
    layout2->setContentsMargins(0,0,0,0);
    this->setLayout(layout2);
    m_red_point->setVisible(false);
}

void StateWidget::showRedPoint(bool show)
{
    m_red_point->setVisible(true);
}

void StateWidget::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    return;
}

void StateWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (m_curState == ClickLbState::Normal)
        {
            m_curState = ClickLbState::Selected;
            setProperty("state", m_selected_press);
            repolish(this);
            update();
            return;
        }
    }

    QLabel::mousePressEvent(event);
}

void StateWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (m_curState == ClickLbState::Normal)
        {
            setProperty("state", m_normal_hover);
            repolish(this);
            update();
        }
        else //Selected
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

void StateWidget::enterEvent(QEnterEvent* event)
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

void StateWidget::leaveEvent(QEvent* event)
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
