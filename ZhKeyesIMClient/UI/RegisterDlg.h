#ifndef UI_REGISTERDLG_H_
#define UI_REGISTERDLG_H_

#include <QDialog>
#include <QTimer>

#include "ui_RegisterDlg.h"

#include "IMClient.h"

class RegisterDlg : public QDialog
{
    Q_OBJECT

public:
    RegisterDlg(std::shared_ptr<IMClient> spClient, QWidget* parent = nullptr);
    ~RegisterDlg();

private:
    void setUpSignals();
    void showTip(const QString& str, bool isError);
    void startCountdown();

signals:
    void switchLoginDlg();

private slots:
    void onCancelButtonClicked();
    void onRegisterButtonClicked();
    void onCodeButtonClicked();

    void onVerifyCodeReceived(bool success, QString message, int errorCode);

private:

    Ui::RegisterDlgClass m_ui;
    // 验证码倒计时
    int m_countdown{ 0 };
    QTimer* m_countdownTimer;
    std::shared_ptr<IMClient> m_spClient;



};

#endif