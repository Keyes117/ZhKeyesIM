#include "TimerButton.h"

#include <QMouseEvent>
#include <QPushButton>

TimerButton::TimerButton(QWidget *parent)
    : QPushButton(parent),
    m_timer(new QTimer(this)),
    m_counter(10)
{
    connect(m_timer, &QTimer::timeout, [this]() {
        m_counter--;
        if (m_counter <= 0)
        {
            m_timer->stop();
            m_counter = 10;
            this->setText("获取");
            this->setEnabled(true);
            return;
        }
        this->setText(QString::number(m_counter));
    });
}

TimerButton::~TimerButton()
{
    m_timer->stop();

}


void TimerButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        this->setEnabled(false);
        this->setText(QString::number(m_counter));
        m_timer->start(1000);
    }

    QPushButton::mouseReleaseEvent(event);
}