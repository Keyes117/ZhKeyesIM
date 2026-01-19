#ifndef ZHKEYESIMCLIENT_UI_CHATDIALOG_H_
#define ZHKEYESIMCLIENT_UI_CHATDIALOG_H_

#include <memory>

#include <QDialog>
#include <QList>
#include <QStackedWidget>

#include "ui_ChatDialog.h"

#include "NetWork/IMClient.h"
#include "UI/ChatUserList.h"
#include "UI/SearchListWidget.h"
#include "Base/global.h"


class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    ChatDialog(std::shared_ptr<IMClient> spClient, QWidget *parent = nullptr);
    ~ChatDialog();

    void addChatUserList();
protected:
    virtual bool eventFilter(QObject* watched, QEvent* event);

private:
    void clearLabelState(StateWidget* label);
    void addLabelGroup(StateWidget* label);
    void showSearch(bool bsearch = false);
    void handleGlobalMousePress(QMouseEvent* event);

private slots:
    void onLineEditSearchChanged(const QString& text);
    void onClearActionTriggered();

    void onLoadingChatUser();

    void onLabelSideChatClicked();
    void onLabelSideContactClicked();



private:
    Ui::ChatDialogClass ui;
    QList<StateWidget*>   m_labelList;


    ChatUserListWidget*  m_chatUserListWidget;
    SearchListWidget* m_searchListWidget;


    ChatUIMode  m_mode = ChatUIMode::ChatMode;
    ChatUIMode  m_state = ChatUIMode::ChatMode;
    bool        m_loading = false;

    std::shared_ptr<IMClient> m_spClient;
    QAction* m_clearAction;
    QAction* m_searchAction;
};

#endif