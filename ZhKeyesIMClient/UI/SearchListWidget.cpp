#include "SearchListWidget.h"

#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>

#include "Base/UserData.h"
#include "UI/AddUserItem.h"
#include "UI/CustomizeEdit.h"
#include "UI/LoadingDialog.h"
#include "UI/FindSuccessDialog.h"
#include "Task/TaskBuilder.h"
#include "Task/TaskHandler.h"
#include "Task/SearchUserTask.h"

SearchListWidget::SearchListWidget(QWidget* parent):
    QListWidget(parent)
{
    Q_UNUSED(parent);

    this->setObjectName("listWidget_search");
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

void SearchListWidget::setSearchEdit(QWidget* edit)
{
    m_searchEdit = edit;
}

bool SearchListWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == this->viewport())
    {
        if (event->type() == QEvent::Enter)
        {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if (event->type() == QEvent::Leave)
        {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    if (watched == this->viewport() && event->type() == QEvent::Wheel)
    {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数
        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);
        return true; 
    }

    return QListWidget::eventFilter(watched, event);
}

void SearchListWidget::addTipItem()
{
    auto* invalid_item = new QWidget();
    QListWidgetItem* item_tmp = new QListWidgetItem;

    item_tmp->setSizeHint(QSize(250, 10));
    this->addItem(item_tmp);

    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);


    auto* add_user_item = new AddUserItem();
    QListWidgetItem* item = new QListWidgetItem();

    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
}

void SearchListWidget::waitPending(bool pending)
{
    if (pending)
    {
        m_loadingDialog = new LoadingDialog(this);
        m_loadingDialog->setModal(true);
        m_loadingDialog->show();
        m_sendPending = pending;
    }
    else
    {
        m_loadingDialog->hide();
        m_loadingDialog->deleteLater();
        m_sendPending = pending;
    }
}

void SearchListWidget::onItemClicked(QListWidgetItem* item)
{
    QWidget* widget = this->itemWidget(item);
    if (!widget)
    {
        return;
    }

    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if (!customItem)
    {
        return;
    }

    auto itemType = customItem->getItemType();
    if (itemType == ListItemType::INVALID_ITEM)
        return;

    if (itemType == ListItemType::ADD_USER_TIP_ITEM)
    {
        if (m_sendPending)
            return;

        if (!m_searchEdit)
            return;

        waitPending(true);
        
        auto searchEdit = dynamic_cast<CustomizeEdit*>(m_searchEdit);
        auto strUid = searchEdit->text();

        uint32_t uid = strUid.toUInt();
        auto task = TaskBuilder::getInstance().buildTask<SearchUserTask>(uid);

        connect(task.get(), &SearchUserTask::userSearched, this, &SearchListWidget::onUserSearch);

        TaskHandler::getInstance().registerNetTask(std::move(task));


    }

    closeFindDlg();
}

void SearchListWidget::onUserSearch(std::shared_ptr<SearchInfo> info)
{

}