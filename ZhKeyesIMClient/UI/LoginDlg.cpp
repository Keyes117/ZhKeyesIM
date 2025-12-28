#include "LoginDlg.h"

#include <QPushButton>
#include <QMessageBox>
#include <QSizePolicy>

#include "ClickedLabel.h"
#include "global.h"

LoginDlg::LoginDlg(std::shared_ptr<IMClient> spClient,QWidget* parent)
    :QDialog(parent),
    m_spClient(spClient)
{
    m_ui.setupUi(this);
    setUpSignals();

    m_ui.label_forgetPwd->setState("normal", "hover", "", "selected", "selected_hover", "");
    m_ui.label_forgetPwd->setCursor(Qt::PointingHandCursor);



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

void LoginDlg::onLoginSuccess(const UserData& userData)
{
    //m_ui.button_login->setEnabled(true);

    QMessageBox::information(this, "成功",
        QString("欢迎回来，%1！").arg(QString::fromStdString(userData.username)));

    // 保存token等信息
    // StateManager::getInstance().setLoginData(data);

    //emit loginSuccess();

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

void LoginDlg::onForgotPasswordLabelClicked()
{
    emit switchResetDlg();

}

void LoginDlg::setUpSignals()
{
    connect(m_ui.button_logon, &QPushButton::clicked, this, &LoginDlg::onLoginButtonClicked);
    connect(m_ui.button_register, &QPushButton::clicked, this, &LoginDlg::onRegisterButtonClicked);

    connect(m_ui.label_forgetPwd, &ClickedLabel::clicked, this, &LoginDlg::onForgotPasswordLabelClicked);
}

