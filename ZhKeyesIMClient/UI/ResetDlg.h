#ifndef ZHKEYESIMCLIENT_UI_RESETDLG_H_
#define ZHKEYESIMCLIENT_UI_RESETDLG_H_

#include <QDialog>
#include "ui_ResetDlg.h"

#include <NetWork/IMClient.h>

class ResetDlg : public QDialog
{
    Q_OBJECT

public:
    ResetDlg( QWidget* parent = nullptr);
    ~ResetDlg();

private:
    // …Ë÷√ ‰»ÎøÚ¥ÌŒÛ◊¥Ã¨
    void setLineEditError(QLineEdit* lineEdit, bool hasError);
    // œ‘ æ/“˛≤ÿ¥ÌŒÛÃ· æ
    void showFieldError(const QString& fieldName, const QString& errorMsg);
    void hideFieldError(const QString& fieldName);

signals:
    void switchLoginDlg();

private slots:

    void onResetPasswordSuccess();
    void onResetPasswordError(const QString& error);

    void onVerifyCodeSuccess();
    void onVerifyCodeError(const QString& error);

    void onUserTextChanged(const QString& text);
    void onEmailTextChanged(const QString& text);
    void onPasswordTextChanged(const QString& text);
    void onCodeTextChanged(const QString& text);

    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkVerifyValid();

   
    void onCancelButtonClicked();
    void onConfirmButtonClicked();
    void onCodeButtonClicked();

private:
    Ui::ResetDlgClass m_ui;
    QMap<QString, QLabel*> m_errorLabels;
};


#endif // UI_RESETDLG_H_

