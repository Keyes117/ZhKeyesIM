#include "LoginDlg.h"

#include <QPushButton>
#include <QMessageBox>
#include <QSizePolicy>

LoginDlg::LoginDlg(std::shared_ptr<IMClient> spClient,QWidget* parent)
    :QDialog(parent),
    m_spClient(spClient)
{
    m_ui.setupUi(this);
    setUpSignals();

}

LoginDlg::~LoginDlg()
{
}

bool LoginDlg::isLineEditEmpty()
{
    return m_ui.lineEdit_accout->text().isEmpty() || m_ui.lineEdit_password->text().isEmpty();
}

bool LoginDlg::isAccountAndPassWordRight()
{
    return false;
}

void LoginDlg::onLoginButtonClicked()
{
    if (isLineEditEmpty())
    {
        QMessageBox::warning(this, "错误", "用户名或密码不能为空", "确定");
    }

    if (!isAccountAndPassWordRight())
    {

    }
}


void LoginDlg::onRegisterButtonClicked()
{
    emit switchRegisterDlg();
}

void LoginDlg::setUpSignals()
{
    connect(m_ui.button_logon, &QPushButton::clicked, this, &LoginDlg::onLoginButtonClicked);
    connect(m_ui.button_register, &QPushButton::clicked, this, &LoginDlg::onRegisterButtonClicked);
}

