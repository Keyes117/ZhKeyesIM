#ifndef UI_REGISTERDLG_H_
#define UI_REGISTERDLG_H_

#include <QDialog>
#include <QTimer>

#include "ui_RegisterDlg.h"

#include "global.h"
#include "NetWork/IMClient.h"

class RegisterDlg : public QDialog
{
    Q_OBJECT

public:
    RegisterDlg(std::shared_ptr<IMClient> spClient, QWidget* parent = nullptr);
    ~RegisterDlg();

private:
    void setUpSignals();
    //void showTip(const QString& str, bool isError);
    //void AddTipErr(TipErr err, QString tips);
    //void DelTipErr(TipErr err);
    void startCountdown();

    // 设置输入框错误状态
    void setLineEditError(QLineEdit* lineEdit, bool hasError);
    // 显示/隐藏错误提示
    void showFieldError(const QString& fieldName, const QString& errorMsg);
    void hideFieldError(const QString& fieldName);


    void onRegisterSuccess(int uid);
    void onRegisterError(const std::string& error);
         
    void onVerifyCodeSuccess();
    void onVerifyCodeError(const std::string& error);

signals:
    void switchLoginDlg();
    void registerSuccess(int uid);

private slots:
    void onCancelButtonClicked();
    void onRegisterButtonClicked();
    void onCodeButtonClicked();

    void onUserTextChanged(const QString& text);
    void onEmailTextChanged(const QString& text);
    void onPasswordTextChanged(const QString& text);
    void onConfirmTextChanged(const QString& text);
    void onCodeTextChanged(const QString& text);

    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkVerifyValid();

    void switchLabelPassVisible();
    void switchLabelConfirmVisible();

private:

    Ui::RegisterDlgClass m_ui;
    // 验证码倒计时
    int m_countdown{ 0 };
    QTimer* m_countdownTimer;
    QMap<QString, QLabel*> m_errorLabels;
    std::shared_ptr<IMClient> m_spClient;



};

#endif