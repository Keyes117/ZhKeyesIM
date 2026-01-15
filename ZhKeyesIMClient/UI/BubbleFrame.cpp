#include "BubbleFrame.h"

#include <QHBoxLayout>
#include <QPainter>

static const int WIDTH_TRIANGLE = 8;

BubbleFrame::BubbleFrame(ChatRole role, QWidget* parent):
    m_role(role),
    QFrame(parent)
{
    m_pHLayout = new QHBoxLayout();
    if (m_role == ChatRole::Self)
        m_pHLayout->setContentsMargins(m_margin, m_margin, WIDTH_TRIANGLE + m_margin, m_margin);
    else
        m_pHLayout->setContentsMargins(WIDTH_TRIANGLE + m_margin, m_margin, m_margin, m_margin);

    this->setLayout(m_pHLayout);
}

void BubbleFrame::setMargin(int margin)
{
    m_margin = margin;
}

void BubbleFrame::setWidget(QWidget* widget)
{
    if (m_pHLayout->count() > 0)
        return;
    else {
        m_pHLayout->addWidget(widget);
    }
}

void BubbleFrame::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    if (m_role == ChatRole::Other)
    {
        //»­ÆøÅÝ
        QColor bk_color(Qt::white);
        painter.setBrush(QBrush(bk_color));
        QRect bk_rect = QRect(WIDTH_TRIANGLE, 0, this->width() - WIDTH_TRIANGLE, this->height());
        painter.drawRoundedRect(bk_rect, 5, 5);
        //»­Ð¡Èý½Ç
        QPointF points[3] = {
            QPointF(bk_rect.x(), 12),
            QPointF(bk_rect.x(), 10 + WIDTH_TRIANGLE + 2),
            QPointF(bk_rect.x() - WIDTH_TRIANGLE, 10 + WIDTH_TRIANGLE - WIDTH_TRIANGLE / 2),
        };
        painter.drawPolygon(points, 3);
    }
    else
    {
        QColor bk_color(158, 234, 106);
        painter.setBrush(QBrush(bk_color));
        //»­ÆøÅÝ
        QRect bk_rect = QRect(0, 0, this->width() - WIDTH_TRIANGLE, this->height());
        painter.drawRoundedRect(bk_rect, 5, 5);
        //»­Èý½Ç
        QPointF points[3] = {
            QPointF(bk_rect.x() + bk_rect.width(), 12),
            QPointF(bk_rect.x() + bk_rect.width(), 12 + WIDTH_TRIANGLE + 2),
            QPointF(bk_rect.x() + bk_rect.width() + WIDTH_TRIANGLE, 10 + WIDTH_TRIANGLE - WIDTH_TRIANGLE / 2),
        };
        painter.drawPolygon(points, 3);
    }
    return QFrame::paintEvent(event);
}
