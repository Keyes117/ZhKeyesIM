#pragma once

#include <QtWidgets/QDialog>

#include "ui_LoginDlg.h"

class LoginDlg : public QDialog
{
    Q_OBJECT
public:
    LoginDlg(QWidget* parent = nullptr);
    ~LoginDlg();

private:
    bool isLineEditEmpty();
    bool isAccountAndPassWordRight();

signals:
    void switchRegisterDlg();

private slots:
    void onLoginButtonClicked();
    void onRegisterButtonClicked();
private:

    void setUpSignals();
    Ui::LoginDlgClass m_ui;
};

