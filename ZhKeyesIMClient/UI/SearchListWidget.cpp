#include "SearchListWidget.h"

SearchListWidget::SearchListWidget(QWidget* parent):
    QListWidget(parent)
{
    Q_UNUSED(parent);

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);
    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &SearchListWidget::onItemClicked);
    //添加条目
    addTipItem();
    //连接搜索条目
    //connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);
}

void SearchListWidget::closeFindDlg()
{
}

void SearchListWidget::setSearchEdit(QWidget* eidt)
{
}

bool SearchListWidget::eventFilter(QObject* watched, QEvent* event)
{
    return false;
}

void SearchListWidget::waitPending(bool pending)
{
}

void SearchListWidget::addTipItem()
{
}

void SearchListWidget::onUserSearch(std::shared_ptr<SearchInfo> info)
{
}
