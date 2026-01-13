#ifndef ZHKEYESIMCLIENT_UI_CHATDIALOG_H_
#define ZHKEYESIMCLIENT_UI_CHATDIALOG_H_

#include <memory>

#include <QDialog>
#include <QStackedWidget>

#include "ui_ChatDialog.h"

#include "NetWork/IMClient.h"
#include "UI/ChatUserList.h"
#include "UI/global.h"


class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    ChatDialog(std::shared_ptr<IMClient> spClient, QWidget *parent = nullptr);
    ~ChatDialog();

    void addChatUserList();

private slots:
    void onLineEditSearchChanged(const QString& text);
    void onClearActionTriggered();

    void onLoadingChatUser();

private:
    Ui::ChatDialogClass ui;

    ChatUserList*    m_chatUserListWidget;

    ChatUIMode  m_mode = ChatUIMode::ChatMode;
    ChatUIMode  m_state = ChatUIMode::ChatMode;
    bool        m_loading = false;

    std::shared_ptr<IMClient> m_spClient;
    QAction* m_clearAction;
    QAction* m_searchAction;
};

#endif