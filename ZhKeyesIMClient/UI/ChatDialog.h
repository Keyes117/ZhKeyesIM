#pragma once

#include <QDialog>
#include "ui_ChatDialog.h"

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

private:
    Ui::ChatDialogClass ui;
};

