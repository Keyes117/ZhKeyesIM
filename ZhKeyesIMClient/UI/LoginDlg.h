#ifndef ZHKEYESIMCLIENT_UI_LOGINDLG_H_
#define ZHKEYESIMCLIENT_UI_LOGINDLG_H_

#include <QtWidgets/QDialog>
#include <QLineEdit>


#include "ui_LoginDlg.h"

#include "Task/UserLoginTask.h"
#include "NetWork/IMClient.h"
class LoginDlg : public QDialog
{
    Q_OBJECT
public:
    LoginDlg( QWidget* parent = nullptr);
    ~LoginDlg();

private:

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

    void onLoginSuccess();
    void onLoginError(const std::string& error);

    bool checkEmailValid();
    bool checkPassValid();

private:

    void setUpSignals();

    QMap<QString, QLabel*> m_errorLabels;
    Ui::LoginDlgClass m_ui;
};

#endif