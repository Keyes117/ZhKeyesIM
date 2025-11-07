#include "LoginDlg.h"

#include <QMessageBox>

LoginDlg::LoginDlg(QWidget* parent)
    : QDialog(parent)
{
    m_ui.setupUi(this);

    this->adjustSize();
    this->setFixedSize(this->sizeHint());
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

}

void LoginDlg::setUpSignals()
{
    connect(m_ui.button_logon, &QPushButton::click, this, &LoginDlg::onLoginButtonClicked);
    connect(m_ui.button_register, &QPushButton::click, this, &LoginDlg::onRegisterButtonClicked);
}

