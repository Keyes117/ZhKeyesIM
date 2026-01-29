#include "LoginDlg.h"

#include <QPushButton>
#include <QMessageBox>
#include <QSizePolicy>
#include <QLineEdit>
#include <QRegularExpression>

#include "Base/UserSession.h"
#include "UI/ClickedLabel.h"
#include "Base/global.h"
#include "Task/TaskHandler.h"
#include "Task/UserLoginTask.h"


LoginDlg::LoginDlg(std::shared_ptr<IMClient> spClient,QWidget* parent)
    :QDialog(parent),
    m_spClient(spClient)
{
    m_ui.setupUi(this);
    setUpSignals();

    m_ui.label_user_error->setVisible(false);
    m_ui.label_password_error->setVisible(false);

    m_ui.label_forgetPwd->setState("normal", "hover", "", "selected", "selected_hover", "");
    m_ui.label_forgetPwd->setCursor(Qt::PointingHandCursor);
    

    connect(m_ui.lineEdit_accout, &QLineEdit::textChanged, this, &LoginDlg::onEmailTextChanged);
    connect(m_ui.lineEdit_password, &QLineEdit::textChanged, this, &LoginDlg::onPasswordTextChanged);


    connect(m_ui.lineEdit_accout, &QLineEdit::editingFinished, this, &LoginDlg::checkEmailValid);
    connect(m_ui.lineEdit_password, &QLineEdit::editingFinished, this, &LoginDlg::checkPassValid);


    m_errorLabels["email"] = m_ui.label_user_error;
    m_errorLabels["password"] = m_ui.label_password_error;




}

LoginDlg::~LoginDlg()
{
}


void LoginDlg::onLoginSuccess()
{
    //m_ui.button_login->setEnabled(true);
    QMessageBox::information(this, "成功",
        QString("欢迎回来，%1！").arg(QString::fromStdString(UserSession::getInstance().getUsername())));

    emit loginSuccess();

}

void LoginDlg::onLoginError(const std::string& error)
{
    QMessageBox::warning(this,
        "注册失败",
        QString::fromStdString(error));
}


void LoginDlg::setLineEditError(QLineEdit* lineEdit, bool hasError)
{
    if (lineEdit) {
        lineEdit->setProperty("error", hasError);
        repolish(lineEdit);
    }
}

void LoginDlg::showFieldError(const QString& fieldName, const QString& errorMsg)
{
    QLabel* errorLabel = m_errorLabels.value(fieldName, nullptr);
    if (errorLabel) {
        errorLabel->setText("⚠ " + errorMsg);
        errorLabel->setProperty("error", true);
        errorLabel->setVisible(true);
        repolish(errorLabel);
    }
}

void LoginDlg::hideFieldError(const QString& fieldName)
{
    QLabel* errorLabel = m_errorLabels.value(fieldName, nullptr);
    if (errorLabel) {
        errorLabel->clear();
        errorLabel->setProperty("error", false);
        errorLabel->setVisible(false);
        repolish(errorLabel);
    }
}



void LoginDlg::onLoginButtonClicked()
{
    emit loginSuccess();

  /*  if (!checkEmailValid())
    {
        return;
    }

    if (!checkPassValid())
    {
        return;
    }

   
    QString email = m_ui.lineEdit_accout->text();
    QString password = m_ui.lineEdit_password->text();

    auto loginTask = std::make_shared<UserLoginTask>(
        m_spClient,
        email.toStdString(),
        password.toStdString(),
        this,
        std::bind(&LoginDlg::onLoginSuccess, this),
        std::bind(&LoginDlg::onLoginError, this, std::placeholders::_1)
    );

    TaskHandler::getInstance().registerNetTask(std::move(loginTask));*/
}


void LoginDlg::onRegisterButtonClicked()
{
    emit switchRegisterDlg();
}

void LoginDlg::onForgotPasswordLabelClicked()
{
    emit switchResetDlg();

}

void LoginDlg::onEmailTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        setLineEditError(m_ui.lineEdit_accout, true);
        showFieldError("email", tr("邮箱不能为空"));
    }
    else {
        QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
        if (!regex.match(text).hasMatch()) {
            setLineEditError(m_ui.lineEdit_accout, true);
            showFieldError("email", tr("邮箱格式不正确"));
        }
        else {
            setLineEditError(m_ui.lineEdit_accout, false);
            hideFieldError("email");
        }
    }
}

void LoginDlg::onPasswordTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        setLineEditError(m_ui.lineEdit_password, true);
        showFieldError("password", tr("密码不能为空"));
    }
    else if (text.length() < 6 || text.length() > 15) {
        setLineEditError(m_ui.lineEdit_password, true);
        showFieldError("password", tr("密码长度应为6~15"));
    }
    else {
        QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
        if (!regExp.match(text).hasMatch()) {
            setLineEditError(m_ui.lineEdit_password, true);
            showFieldError("password", tr("不能包含非法字符"));
        }
        else {
            setLineEditError(m_ui.lineEdit_password, false);
            hideFieldError("password");
        }
    }
}

bool LoginDlg::checkEmailValid()
{
    QString email = m_ui.lineEdit_accout->text();
    if (email.isEmpty()) {
        setLineEditError(m_ui.lineEdit_accout, true);
        showFieldError("email", tr("邮箱不能为空"));
        return false;
    }

    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    if (!regex.match(email).hasMatch()) {
        setLineEditError(m_ui.lineEdit_accout, true);
        showFieldError("email", tr("邮箱地址不正确"));
        return false;
    }

    setLineEditError(m_ui.lineEdit_accout, false);
    hideFieldError("email");
    return true;
}

bool LoginDlg::checkPassValid()
{
    QString pass = m_ui.lineEdit_password->text();
    if (pass.isEmpty()) {
        setLineEditError(m_ui.lineEdit_password, true);
        showFieldError("password", tr("密码不能为空"));
        return false;
    }

    if (pass.length() < 6 || pass.length() > 15) {
        setLineEditError(m_ui.lineEdit_password, true);
        showFieldError("password", tr("密码长度应为6~15"));
        return false;
    }

    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    if (!regExp.match(pass).hasMatch()) {
        setLineEditError(m_ui.lineEdit_password, true);
        showFieldError("password", tr("不能包含非法字符"));
        return false;
    }

    setLineEditError(m_ui.lineEdit_password, false);
    hideFieldError("password");
    return true;
}

void LoginDlg::setUpSignals()
{
    connect(m_ui.button_logon, &QPushButton::clicked, this, &LoginDlg::onLoginButtonClicked);
    connect(m_ui.button_register, &QPushButton::clicked, this, &LoginDlg::onRegisterButtonClicked);

    connect(m_ui.label_forgetPwd, &ClickedLabel::clicked, this, &LoginDlg::onForgotPasswordLabelClicked);
}

