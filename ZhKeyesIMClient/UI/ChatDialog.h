#pragma once

#include <memory>

#include <QDialog>
#include "ui_ChatDialog.h"

#include "NetWork/IMClient.h"

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    ChatDialog(std::shared_ptr<IMClient> spClient, QWidget *parent = nullptr);
    ~ChatDialog();


private slots:
    void onLineEditSearchChanged(const QString& text);
    void onClearActionTriggered();

private:
    Ui::ChatDialogClass ui;

    std::shared_ptr<IMClient> m_spClient;
    QAction* m_clearAction;
    QAction* m_searchAction;
};

