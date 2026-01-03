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

    void onLoginSuccess(const User& userData);
    void onLoginError(const std::string& error);

    // …Ë÷√ ‰»ÎøÚ¥ÌŒÛ◊¥Ã¨
    void setLineEditError(QLineEdit* lineEdit, bool hasError);
    // œ‘ æ/“˛≤ÿ¥ÌŒÛÃ· æ
    void showFieldError(const QString& fieldName, const QString& errorMsg);
    void hideFieldError(const QString& fieldName);

signals:
    void switchRegisterDlg();
    void switchResetDlg();
    void loginSuccess();

private slots:
    void onLoginButtonClicked();
    void onRegisterButtonClicked();
    void onForgotPasswordLabelClicked();


    void onEmailTextChanged(const QString& text);
    void onPasswordTextChanged(const QString& text);


    bool checkEmailValid();
    bool checkPassValid();

private:

    void setUpSignals();
    std::shared_ptr<IMClient> m_spClient;

    QMap<QString, QLabel*> m_errorLabels;
    Ui::LoginDlgClass m_ui;
};

#endif