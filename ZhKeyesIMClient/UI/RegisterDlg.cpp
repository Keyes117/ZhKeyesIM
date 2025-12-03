#include "RegisterDlg.h"



#include <QRegularExpression>
#include <QTimer>

#include "global.h"
#include "TaskHandler.h"
#include "GetVerifyCodeTask.h"

RegisterDlg::RegisterDlg(std::shared_ptr<IMClient> spClient, QWidget* parent)
    : QDialog(parent),
    m_countdownTimer(new QTimer(this)),
    m_spClient(spClient)
{
    m_ui.setupUi(this);
    setUpSignals();
    m_ui.lineEdit_password->setEchoMode(QLineEdit::Password);
    m_ui.lineEdit_confirm->setEchoMode(QLineEdit::Password);

    m_ui.err_tip->setProperty("state", "normal");
    repolish(m_ui.err_tip);
}

RegisterDlg::~RegisterDlg()
{
}

void RegisterDlg::setUpSignals()
{
    connect(m_ui.button_cancel, &QPushButton::clicked, this,&RegisterDlg::onCancelButtonClicked);
    connect(m_ui.button_register, &QPushButton::clicked, this, &RegisterDlg::onRegisterButtonClicked);
    connect(m_ui.button_code, &QPushButton::clicked, this, &RegisterDlg::onCodeButtonClicked);

    connect(&TaskHandler::getInstance(), &TaskHandler::verifyCodeRecevied, this, &RegisterDlg::onVerifyCodeReceived);
}

void RegisterDlg::showTip(const QString& str, bool isError)
{
    m_ui.err_tip->setText(str);
    if(isError)
        m_ui.err_tip->setProperty("state", "err");
    else
        m_ui.err_tip->setProperty("state", "normal");
    repolish(m_ui.err_tip);
}

void RegisterDlg::startCountdown()
{
    m_countdown = 60;  // 60秒倒计时
    m_ui.button_code->setEnabled(false);
    m_ui.button_code->setText(QString("重新发送(%1)").arg(m_countdown));
    m_countdownTimer->start(1000);  // 每秒触发一次
}


void RegisterDlg::onCancelButtonClicked()
{
    emit switchLoginDlg();
}
void RegisterDlg::onRegisterButtonClicked()
{

}

void RegisterDlg::onCodeButtonClicked()
{
    QString email = m_ui.lineEdit_emal->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if (match)
    {
        //发送验证码
        auto verifyCodeTask = std::make_shared<GetVerifyCodeTask>(m_spClient, email.toStdString());
        TaskHandler::getInstance().registerNetTask(std::move(verifyCodeTask));
    }
    else
    {
        showTip(tr("邮箱格式不正确"),true);
    }
}

void RegisterDlg::onVerifyCodeReceived(bool success, QString message, int errorCode)
{
    if (success)
    {
        showTip("验证码已发送到您的邮箱，请查收", false);
    }
    else
    {
        showTip("验证码发送失败: " + message, true);
    }
}
