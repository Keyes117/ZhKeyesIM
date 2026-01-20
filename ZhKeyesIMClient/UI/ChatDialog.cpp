#include "ChatDialog.h"

#include "Base/global.h"
#include "ClickedButton.h"
#include "ChatUserItem.h"

#include <QRandomGenerator>
#include <QAction>
#include <QIcon>


ChatDialog::ChatDialog(std::shared_ptr<IMClient> spClient, QWidget* parent) :
    m_spClient(spClient), QDialog(parent)
{
    ui.setupUi(this);

    m_chatUserListWidget = new ChatUserListWidget(ui.stackedWidget_user);
    m_chatUserListWidget->setWindowFlag(Qt::Widget);

    m_searchListWidget = new SearchListWidget(ui.stackedWidget_user);
    m_searchListWidget->setWindowFlag(Qt::Widget);

    m_applyFriendListWidget = new ApplyFriendListWidget(ui.stackedWidget_user);
    m_applyFriendListWidget->setWindowFlag(Qt::Widget);

    m_contactUserListWidget = new ContactUserListWidget(ui.stackedWidget_user);
    m_contactUserListWidget->setWindowFlag(Qt::Widget);
 


    ui.stackedWidget_user->addWidget(m_chatUserListWidget);
    ui.stackedWidget_user->addWidget(m_searchListWidget);
    ui.stackedWidget_user->addWidget(m_applyFriendListWidget);
    ui.stackedWidget_user->addWidget(m_contactUserListWidget);

    QPixmap pixmap(":/res/res/head_1.jpg");
    ui.label_side_head->setPixmap(pixmap);

    QPixmap scaledPixmap = pixmap.scaled(ui.label_side_head->size(), Qt::KeepAspectRatio);
    ui.label_side_head->setPixmap(scaledPixmap);
    ui.label_side_head->setScaledContents(true);

    ui.label_side_chat->setProperty("state", "normal");
    ui.label_side_chat->setState("normal", "hover", "press", 
        "selected_normal", "selected_hover", "selected_press");
    ui.label_side_contact->setState("normal", "hover", "press",
        "selected_normal", "selected_hover", "selected_press");

    //ui.label_side_chat->setSelected(false);..

    ui.button_add->SetState("normal", "hover", "press");

    m_searchAction = new QAction(ui.lineEdit_search);
    m_searchAction->setIcon(QIcon(":/res/res/search.png"));
    ui.lineEdit_search->addAction(m_searchAction, QLineEdit::LeadingPosition);
    ui.lineEdit_search->setPlaceholderText(QString::fromLocal8Bit("搜索"));

    m_clearAction = new QAction(ui.lineEdit_search);
    m_clearAction->setIcon(QIcon(":/res/res/close_transparent.png"));

    ui.lineEdit_search->addAction(m_clearAction, QLineEdit::TrailingPosition);

    connect(ui.label_side_chat, &StateWidget::clicked, this, &ChatDialog::onLabelSideChatClicked);
    connect(ui.label_side_contact, &StateWidget::clicked, this, &ChatDialog::onLabelSideContactClicked);
    connect(ui.lineEdit_search, &QLineEdit::textChanged, this, &ChatDialog::onLineEditSearchChanged);
    connect(m_clearAction, &QAction::triggered, this, &ChatDialog::onClearActionTriggered);
    //connect(ui.stackedWidget_chat.pag)

    addLabelGroup(ui.label_side_chat);
    addLabelGroup(ui.label_side_contact);

    this->installEventFilter(this);

    ui.label_side_chat->setSelected(true);

    ui.lineEdit_search->setMaxLength(15);

    addChatUserList();
    
}

ChatDialog::~ChatDialog()
{}

void ChatDialog::addChatUserList()
{
    // 创建QListWidgetItem，并设置自定义的widget
    for (int i = 0; i < 13; i++) {
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue % strs.size();
        int head_i = randomValue % heads.size();
        int name_i = randomValue % names.size();
        auto* chat_user_wid = new ChatUserItem();
        chat_user_wid->setInfo(names[name_i], heads[head_i], strs[str_i]);
        QListWidgetItem* item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
       m_chatUserListWidget->addItem(item);
       m_chatUserListWidget->setItemWidget(item, chat_user_wid);
    }
}

bool ChatDialog::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
        handleGlobalMousePress(mouseEvent);
    }

    return QDialog::eventFilter(watched, event);
}

void ChatDialog::clearLabelState(StateWidget* label)
{
    for (auto& pLabel : m_labelList)
    {
        if (pLabel == label)
            continue;

        pLabel->clearState();
    }

}

void ChatDialog::addLabelGroup(StateWidget* label)
{
    m_labelList.push_back(label);
}

void ChatDialog::showSearch(bool bsearch)
{
    if (bsearch)
    {
        ui.stackedWidget_user->setCurrentWidget(m_searchListWidget);
        m_mode = ChatUIMode::SearchMode;
    }
    else if (m_state == ChatUIMode::SearchMode)
    {
        ui.stackedWidget_user->setCurrentWidget(m_searchListWidget);
        m_mode = ChatUIMode::SearchMode;
    }
    else if (m_state == ChatUIMode::ContactMode)
    {
        m_mode = ChatUIMode::ContactMode;
        ui.stackedWidget_user->setCurrentWidget(m_contactUserListWidget);
    }
    else if (m_state == ChatUIMode::ChatMode)
    {
        ui.stackedWidget_user->setCurrentWidget(m_chatUserListWidget);
        m_mode = ChatUIMode::ChatMode;
    }
    else if (m_state == ChatUIMode::SettingsMode)
    {
        m_mode = ChatUIMode::SettingsMode;
    }
}

void ChatDialog::handleGlobalMousePress(QMouseEvent* event)
{
    if (m_mode != ChatUIMode::SearchMode)
        return;

    QPoint posInSearchList = m_searchListWidget->mapFromGlobal(event->globalPos());

    if (!m_searchListWidget->rect().contains(posInSearchList))
    {
        ui.lineEdit_search->clear();
        showSearch(false);
    }
}


void ChatDialog::onLineEditSearchChanged(const QString& text)
{
    if (!text.isEmpty())
    {
        m_clearAction->setIcon(QIcon(":/res/res/close_search.png"));
        showSearch(true);
    }
    else
    {
        m_clearAction->setIcon(QIcon(":/res/res/close_transparent.png"));
        showSearch(false);
    }

}
void ChatDialog::onClearActionTriggered()
{
    ui.lineEdit_search->clear();
    m_clearAction->setIcon(QIcon(":/res/res/close_transparent.png"));
    ui.lineEdit_search->clearFocus();

}

void ChatDialog::onLoadingChatUser()
{

}

void ChatDialog::onLabelSideChatClicked()
{
    clearLabelState(ui.label_side_chat);
    ui.stackedWidget_chat->setCurrentWidget(ui.page_chatPage);
    m_state = ChatUIMode::ChatMode;
    showSearch(false);
}

void ChatDialog::onLabelSideContactClicked()
{
    clearLabelState(ui.label_side_contact);
    m_state = ChatUIMode::ContactMode;
    showSearch(false);

}
