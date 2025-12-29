#ifndef UI_RESETDLG_H_
#define UI_RESETDLG_H_

#include <QDialog>
#include "ui_ResetDlg.h"

#include <IMClient.h>

class ResetDlg : public QDialog
{
    Q_OBJECT

public:
    ResetDlg(std::shared_ptr<IMClient> spClient, QWidget* parent = nullptr);
    ~ResetDlg();


    void onResetPasswordSuccess();
    void onResetPasswordError(const std::string& error);

    void onVerifyCodeSuccess();
    void onVerifyCodeError(const std::string& error);

private:
    // …Ë÷√ ‰»ÎøÚ¥ÌŒÛ◊¥Ã¨
    void setLineEditError(QLineEdit* lineEdit, bool hasError);
    // œ‘ æ/“˛≤ÿ¥ÌŒÛÃ· æ
    void showFieldError(const QString& fieldName, const QString& errorMsg);
    void hideFieldError(const QString& fieldName);

signals:
    void switchLoginDlg();

private slots:
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

    std::shared_ptr<IMClient> m_spClient;

    QMap<QString, QLabel*> m_errorLabels;
};


#endif // UI_RESETDLG_H_

