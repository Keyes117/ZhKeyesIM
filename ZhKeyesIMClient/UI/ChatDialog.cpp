#include "ChatDialog.h"

#include "ClickedButton.h"
#include "ChatUserWidget.h"

#include <QRandomGenerator>
#include <QAction>
#include <QIcon>

std::vector<QString>  strs = { "hello world !",
                             "nice to meet u",
                             "New year，new life",
                            "You have to love yourself",
                            "My love is written in the wind ever since the whole world is you" };
std::vector<QString> heads = {
    ":/res/res/head_1.jpg",
    ":/res/res/head_2.jpg",
    ":/res/res/head_3.jpg",
    ":/res/res/head_4.jpg",
    ":/res/res/head_5.jpg"
};
std::vector<QString> names = {
    "llfc",
    "zack",
    "golang",
    "cpp",
    "java",
    "nodejs",
    "python",
    "rust"
};

ChatDialog::ChatDialog(std::shared_ptr<IMClient> spClient, QWidget* parent) :
    m_spClient(spClient), QDialog(parent)
{
    ui.setupUi(this);

    m_chatUserListWidget = new ChatUserList(ui.stackedWidget_user);
    m_chatUserListWidget->setWindowFlag(Qt::Widget);

    ui.stackedWidget_user->addWidget(m_chatUserListWidget);

    ui.button_add->SetState("normal", "hover", "press");

    m_searchAction = new QAction(ui.lineEdit_search);
    m_searchAction->setIcon(QIcon(":/res/res/search.png"));
    ui.lineEdit_search->addAction(m_searchAction, QLineEdit::LeadingPosition);
    ui.lineEdit_search->setPlaceholderText(QString::fromLocal8Bit("搜索"));

    m_clearAction = new QAction(ui.lineEdit_search);
    m_clearAction->setIcon(QIcon(":/res/res/close_transparent.png"));

    ui.lineEdit_search->addAction(m_clearAction, QLineEdit::TrailingPosition);

    connect(ui.lineEdit_search, &QLineEdit::textChanged, this, &ChatDialog::onLineEditSearchChanged);
    connect(m_clearAction, &QAction::triggered, this, &ChatDialog::onClearActionTriggered);

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
        auto* chat_user_wid = new ChatUserWidget();
        chat_user_wid->setInfo(names[name_i], heads[head_i], strs[str_i]);
        QListWidgetItem* item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
       m_chatUserListWidget->addItem(item);
       m_chatUserListWidget->setItemWidget(item, chat_user_wid);
    }
}


void ChatDialog::onLineEditSearchChanged(const QString& text)
{
    if (!text.isEmpty())
    {
        m_clearAction->setIcon(QIcon(":/res/res/close_search.png"));
    }
    else
    {
        m_clearAction->setIcon(QIcon(":/res/res/close_transparent.png"));
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
