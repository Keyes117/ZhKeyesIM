#pragma once

#include <QDialog>
#include "ui_FindSuccessDialog.h"

#include <memory>
#include "Base/UserData.h"

class FindSuccessDialog : public QDialog
{
    Q_OBJECT

public:
    FindSuccessDialog(QWidget *parent = nullptr);
    ~FindSuccessDialog();

    void SetSearchInfo(std::shared_ptr<SearchInfo> searchInfo);

private slots:
    void onButtonAddFriendClicked();

private:
    Ui::FindSuccessDialogClass ui;

    QWidget* m_parent;
    std::shared_ptr<SearchInfo> m_searchInfo;
};

