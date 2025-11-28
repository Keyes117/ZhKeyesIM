#include "RegisterDlg.h"


#include <QRegularExpression>

#include "global.h"


RegisterDlg::RegisterDlg(std::shared_ptr<IMClient> spClient, QWidget* parent)
    : QDialog(parent),
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

    }
    else
    {
        showTip(tr("邮箱格式不正确"),true);
    }
}
