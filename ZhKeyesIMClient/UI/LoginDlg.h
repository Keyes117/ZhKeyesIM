#ifndef UI_LOGINDLG_H_
#define UI_LOGINDLG_H_

#include <QtWidgets/QDialog>

#include "Task/UserLoginTask.h"
#include "ui_LoginDlg.h"

#include "IMClient.h"
class LoginDlg : public QDialog
{
    Q_OBJECT
public:
    LoginDlg(std::shared_ptr<IMClient> spClient, QWidget* parent = nullptr);
    ~LoginDlg();

private:
    bool isLineEditEmpty();
    bool isAccountAndPassWordRight();

    void onLoginSuccess(const UserData& userData);
    void onLoginError(const QString& error);

signals:
    void switchRegisterDlg();
    void switchResetDlg();
    void loginSuccess();

private slots:
    void onLoginButtonClicked();
    void onRegisterButtonClicked();
    void onForgotPasswordLabelClicked();
private:

    void setUpSignals();
    std::shared_ptr<IMClient> m_spClient;

    Ui::LoginDlgClass m_ui;
};

#endif