#include "ChatView.h"

#include <QEvent>
#include <QPainter>
#include <QScrollBar>
#include <QStyleOption>
#include <QTimer>

ChatView::ChatView(QWidget* parent):QWidget(parent)
{
    QVBoxLayout* pMainLayout = new QVBoxLayout();
    this->setLayout(pMainLayout);
    pMainLayout->setContentsMargins(QMargins(0, 0, 0, 0));

    m_pScrollArea = new QScrollArea();
    m_pScrollArea->setObjectName("scrollArea_chat");
    pMainLayout->addWidget(m_pScrollArea);

    QWidget* widget = new QWidget(this);
    widget->setObjectName("widget_chat");
    widget->setAutoFillBackground(true);

    QVBoxLayout* pLayout_chat = new QVBoxLayout();
    pLayout_chat->addWidget(new QWidget(), 100000);
    widget->setLayout(pLayout_chat);
    m_pScrollArea->setWidget(widget);

    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScrollBar* pVScrollBar = m_pScrollArea->verticalScrollBar();
    connect(pVScrollBar, &QScrollBar::rangeChanged, this, &ChatView::onVScrollBarMoved);

    QHBoxLayout* pHLayout_chat = new QHBoxLayout();
    pHLayout_chat->addWidget(pVScrollBar, 0, Qt::AlignRight);
    pHLayout_chat->setContentsMargins(0, 0, 0, 0);
    m_pScrollArea->setLayout(pHLayout_chat);
    pVScrollBar->setHidden(true);

    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->installEventFilter(this);
    initStyleSheet();
}

void ChatView::appendChatItem(QWidget* item)
{
    QVBoxLayout* pVLayout = qobject_cast<QVBoxLayout*>(m_pScrollArea->widget()->layout());
    pVLayout->insertWidget(pVLayout->count() - 1, item);
    m_isAppended = true;
}

void ChatView::prependChatItem(QWidget* item)
{
}

void ChatView::insertChatItem(QWidget* before, QWidget* item)
{
}

bool ChatView::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::Enter && watched == m_pScrollArea)
    {
        m_pScrollArea->verticalScrollBar()->setHidden(m_pScrollArea->verticalScrollBar()->maximum() == 0);
    }
    else if(event->type() == QEvent::Leave && watched == m_pScrollArea)
    {
        m_pScrollArea->verticalScrollBar()->setHidden(true);
    }

    return QWidget::eventFilter(watched, event);
}

void ChatView::paintEvent(QPaintEvent* event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatView::initStyleSheet()
{
}


void ChatView::onVScrollBarMoved(int min, int max)
{
    if (m_isAppended) //添加item可能调用多次
    {
        QScrollBar* pVScrollBar = m_pScrollArea->verticalScrollBar();
        pVScrollBar->setSliderPosition(pVScrollBar->maximum());
        //500毫秒内可能调用多次
        QTimer::singleShot(500, [this]()
            {
                m_isAppended = false;
            });
    }
}