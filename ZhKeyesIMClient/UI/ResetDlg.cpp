#include "ResetDlg.h"

#include <QRegularExpression>



#include "Base/global.h"

#include "Task/TaskHandler.h"
#include "Task/TaskBuilder.h"

ResetDlg::ResetDlg(QWidget *parent)
    : QDialog(parent)
{
    m_ui.setupUi(this);
    m_ui.label_user_error->setVisible(false);
    m_ui.label_email_error->setVisible(false);
    m_ui.label_code_error->setVisible(false);
    m_ui.label_password_error->setVisible(false);

    connect(m_ui.button_cancel, &QPushButton::clicked, this, &ResetDlg::onCancelButtonClicked);
    connect(m_ui.button_confirm, &QPushButton::clicked, this, &ResetDlg::onConfirmButtonClicked);
    connect(m_ui.button_code, &QPushButton::clicked, this, &ResetDlg::onCodeButtonClicked);

    connect(m_ui.lineEdit_user, &QLineEdit::textChanged, this, &ResetDlg::onUserTextChanged);
    connect(m_ui.lineEdit_email, &QLineEdit::textChanged, this, &ResetDlg::onEmailTextChanged);
    connect(m_ui.lineEdit_password, &QLineEdit::textChanged, this, &ResetDlg::onPasswordTextChanged);
    connect(m_ui.lineEdit_code, &QLineEdit::textChanged, this, &ResetDlg::onCodeTextChanged);

    connect(m_ui.lineEdit_user, &QLineEdit::editingFinished, this, &ResetDlg::checkUserValid);
    connect(m_ui.lineEdit_email, &QLineEdit::editingFinished, this, &ResetDlg::checkEmailValid);
    connect(m_ui.lineEdit_password, &QLineEdit::editingFinished, this, &ResetDlg::checkPassValid);
    connect(m_ui.lineEdit_code, &QLineEdit::editingFinished, this, &ResetDlg::checkVerifyValid);

    m_errorLabels["user"] = m_ui.label_user_error;
    m_errorLabels["email"] = m_ui.label_email_error;
    m_errorLabels["password"] = m_ui.label_password_error;
    m_errorLabels["code"] = m_ui.label_code_error;
}

ResetDlg::~ResetDlg()
{}

void ResetDlg::onResetPasswordSuccess()
{

}

void ResetDlg::onResetPasswordError(const std::string& error)
{

}

void ResetDlg::onVerifyCodeSuccess()
{

}

void ResetDlg::onVerifyCodeError(const std::string& error)
{

}


void ResetDlg::setLineEditError(QLineEdit* lineEdit, bool hasError)
{
    if (lineEdit) {
        lineEdit->setProperty("error", hasError);
        repolish(lineEdit);
    }
}

void ResetDlg::showFieldError(const QString& fieldName, const QString& errorMsg)
{
    QLabel* errorLabel = m_errorLabels.value(fieldName, nullptr);
    if (errorLabel) {
        errorLabel->setText("⚠ " + errorMsg);
        errorLabel->setProperty("error", true);
        errorLabel->setVisible(true);
        repolish(errorLabel);
    }
}

void ResetDlg::hideFieldError(const QString& fieldName)
{
    QLabel* errorLabel = m_errorLabels.value(fieldName, nullptr);
    if (errorLabel) {
        errorLabel->clear();
        errorLabel->setProperty("error", false);
        errorLabel->setVisible(false);
        repolish(errorLabel);
    }
}


void ResetDlg::onUserTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        setLineEditError(m_ui.lineEdit_user, true);
        showFieldError("user", tr("昵称不可以为空"));
    }
    else {
        setLineEditError(m_ui.lineEdit_user, false);
        hideFieldError("user");
    }
}

void ResetDlg::onEmailTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        setLineEditError(m_ui.lineEdit_email, true);
        showFieldError("email", tr("邮箱不能为空"));
    }
    else {
        QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
        if (!regex.match(text).hasMatch()) {
            setLineEditError(m_ui.lineEdit_email, true);
            showFieldError("email", tr("邮箱格式不正确"));
        }
        else {
            setLineEditError(m_ui.lineEdit_email, false);
            hideFieldError("email");
        }
    }
}

void ResetDlg::onPasswordTextChanged(const QString& text)
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

void ResetDlg::onCodeTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        setLineEditError(m_ui.lineEdit_code, true);
        showFieldError("code", tr("验证码不能为空"));
    }
    else {
        setLineEditError(m_ui.lineEdit_code, false);
        hideFieldError("code");
    }
}

bool ResetDlg::checkUserValid()
{
    QString text = m_ui.lineEdit_user->text();
    if (text.isEmpty()) {
        setLineEditError(m_ui.lineEdit_user, true);
        showFieldError("user", tr("昵称不可以为空"));
        return false;
    }
    setLineEditError(m_ui.lineEdit_user, false);
    hideFieldError("user");
    return true;
}

bool ResetDlg::checkEmailValid()
{
    QString email = m_ui.lineEdit_email->text();
    if (email.isEmpty()) {
        setLineEditError(m_ui.lineEdit_email, true);
        showFieldError("email", tr("邮箱不能为空"));
        return false;
    }

    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    if (!regex.match(email).hasMatch()) {
        setLineEditError(m_ui.lineEdit_email, true);
        showFieldError("email", tr("邮箱地址不正确"));
        return false;
    }

    setLineEditError(m_ui.lineEdit_email, false);
    hideFieldError("email");
    return true;
}

bool ResetDlg::checkPassValid()
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


bool ResetDlg::checkVerifyValid()
{
    QString code = m_ui.lineEdit_code->text();
    if (code.isEmpty()) {
        setLineEditError(m_ui.lineEdit_code, true);
        showFieldError("code", tr("验证码不能为空"));
        return false;
    }

    setLineEditError(m_ui.lineEdit_code, false);
    hideFieldError("code");
    return true;
}

void ResetDlg::onCancelButtonClicked()
{
    emit switchLoginDlg();
}

void ResetDlg::onConfirmButtonClicked()
{
    if (!checkUserValid())
        return;

    if (!checkEmailValid())
        return;

    if (!checkPassValid())
        return;

    if (!checkVerifyValid())
        return;

    QString user = m_ui.lineEdit_user->text();
    QString email = m_ui.lineEdit_email->text();
    QString code = m_ui.lineEdit_code->text();
    QString password = m_ui.lineEdit_password->text();

    auto resetTask = TaskBuilder::getInstance().buildResetPasswordTask(
        email.toStdString(),
        password.toStdString(),
        code.toStdString());

    connect(resetTask.get(), Task::taskSuccess, this, ResetDlg::onResetPasswordSuccess);
    connect(resetTask.get(), Task::taskFailed, this, ResetDlg::onResetPasswordError);

    TaskHandler::getInstance().registerNetTask(std::move(resetTask));
}

void ResetDlg::onCodeButtonClicked()
{
    if (!checkEmailValid())
        return;

    QString email = m_ui.lineEdit_email->text();

    auto codeTask = TaskBuilder::getInstance().buildVerifyCodeTask(email.toStdString());

    connect(codeTask.get(), Task::taskSuccess, this, ResetDlg::onVerifyCodeSuccess);
    connect(codeTask.get(), Task::taskFailed, this, ResetDlg::onVerifyCodeError);

    TaskHandler::getInstance().registerNetTask(std::move(codeTask));
}
