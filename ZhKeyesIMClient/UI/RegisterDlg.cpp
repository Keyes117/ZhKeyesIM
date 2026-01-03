#include "RegisterDlg.h"


#include <QMessageBox>
#include <QRegularExpression>
#include <QTimer>

#include "global.h"
#include "TaskHandler.h"
#include "Task/RegisterTask.h"
#include "Task/VerifyCodeTask.h"

RegisterDlg::RegisterDlg(std::shared_ptr<IMClient> spClient, QWidget* parent)
    : QDialog(parent),
    m_countdownTimer(new QTimer(this)),
    m_spClient(spClient)
{
    m_ui.setupUi(this);
    setUpSignals();
    m_ui.lineEdit_password->setEchoMode(QLineEdit::Password);
    m_ui.lineEdit_confirm->setEchoMode(QLineEdit::Password);

    //m_ui.err_tip->setVisible(false);
    m_ui.label_userErr->setVisible(false);
    m_ui.label_emailErr->setVisible(false);

    m_ui.label_passwordErr->setVisible(false);
    m_ui.label_confirmErr->setVisible(false);
    m_ui.label_codeErr->setVisible(false);


    //m_ui.err_tip->setProperty("state", "normal");
    //repolish(m_ui.err_tip);

    m_ui.label_pass_visible->setState("unvisible", "unvisible_hover", "", "visible", "visible_hover", "");
    m_ui.label_confirm_visible->setState("unvisible", "unvisible_hover", "", "visible", "visible_hover", "");
    m_ui.label_pass_visible->setCursor(Qt::PointingHandCursor);
    m_ui.label_confirm_visible->setCursor(Qt::PointingHandCursor);


    m_errorLabels["user"] = m_ui.label_userErr;
    m_errorLabels["email"] = m_ui.label_emailErr;
    m_errorLabels["password"] = m_ui.label_passwordErr;
    m_errorLabels["confirm"] = m_ui.label_confirmErr;
    m_errorLabels["code"] = m_ui.label_codeErr;

}

RegisterDlg::~RegisterDlg()
{
}

void RegisterDlg::setUpSignals()
{
    connect(m_ui.button_cancel, &QPushButton::clicked, this,&RegisterDlg::onCancelButtonClicked);
    connect(m_ui.button_register, &QPushButton::clicked, this, &RegisterDlg::onRegisterButtonClicked);
    connect(m_ui.button_code, &QPushButton::clicked, this, &RegisterDlg::onCodeButtonClicked);

    connect(m_ui.lineEdit_user, &QLineEdit::textChanged, this, &RegisterDlg::onUserTextChanged);
    connect(m_ui.lineEdit_email, &QLineEdit::textChanged, this, &RegisterDlg::onEmailTextChanged);
    connect(m_ui.lineEdit_password, &QLineEdit::textChanged, this, &RegisterDlg::onPasswordTextChanged);
    connect(m_ui.lineEdit_confirm, &QLineEdit::textChanged, this, &RegisterDlg::onConfirmTextChanged);
    connect(m_ui.lineEdit_code, &QLineEdit::textChanged, this, &RegisterDlg::onCodeTextChanged);

    connect(m_ui.lineEdit_user, &QLineEdit::editingFinished, this, &RegisterDlg::checkUserValid);
    connect(m_ui.lineEdit_email, &QLineEdit::editingFinished, this, &RegisterDlg::checkEmailValid);
    connect(m_ui.lineEdit_password, &QLineEdit::editingFinished, this, &RegisterDlg::checkPassValid);
    connect(m_ui.lineEdit_confirm, &QLineEdit::editingFinished, this, &RegisterDlg::checkConfirmValid);
    connect(m_ui.lineEdit_code, &QLineEdit::editingFinished, this, &RegisterDlg::checkVerifyValid);

    connect(m_ui.label_pass_visible, &ClickedLabel::clicked, this, &RegisterDlg::switchLabelPassVisible);
    connect(m_ui.label_confirm_visible, &ClickedLabel::clicked, this, &RegisterDlg::switchLabelConfirmVisible);

    //connect(&TaskHandler::getInstance(), &TaskHandler::verifyCodeRecevied, this, &RegisterDlg::onVerifyCodeReceived);
}

void RegisterDlg::startCountdown()
{
    m_countdown = 60;  // 60秒倒计时
    m_ui.button_code->setEnabled(false);
    m_ui.button_code->setText(QString("重新发送(%1)").arg(m_countdown));
    m_countdownTimer->start(1000);  // 每秒触发一次
}

void RegisterDlg::setLineEditError(QLineEdit* lineEdit, bool hasError)
{
    if (lineEdit) {
        lineEdit->setProperty("error", hasError);
        repolish(lineEdit);
    }
}

void RegisterDlg::showFieldError(const QString& fieldName, const QString& errorMsg)
{
    QLabel* errorLabel = m_errorLabels.value(fieldName, nullptr);
    if (errorLabel) {
        errorLabel->setText("⚠ " + errorMsg);
        errorLabel->setProperty("error", true);
        errorLabel->setVisible(true);
        repolish(errorLabel);
    }
}

void RegisterDlg::hideFieldError(const QString& fieldName)
{
    QLabel* errorLabel = m_errorLabels.value(fieldName, nullptr);
    if (errorLabel) {
        errorLabel->clear();
        errorLabel->setProperty("error", false);
        errorLabel->setVisible(false);
        repolish(errorLabel);
    }
}

void RegisterDlg::onRegisterSuccess(int uid)
{
    //showLoading(false);

    QMessageBox::information(this,
        "注册成功",
        QString("欢迎！您的ID是：%1").arg(uid));

    emit registerSuccess(uid);
    emit switchLoginDlg();
}

void RegisterDlg::onRegisterError(const std::string& error)
{
    //showLoading(false);

    QMessageBox::warning(this,
        "注册失败",
        QString::fromStdString(error));
}

void RegisterDlg::onVerifyCodeSuccess()
{
    m_ui.button_code->setEnabled(true);
    m_ui.button_code->setText("获取验证码");

    QMessageBox::information(this,
        "成功",
        "验证码已发送到您的邮箱，请注意查收");

    //startCountdown();
}

void RegisterDlg::onVerifyCodeError(const std::string& error) 
{
    m_ui.button_code->setEnabled(true);
    m_ui.button_code->setText("获取验证码");

    QMessageBox::warning(this,
        "错误",
        QString::fromStdString(error));
}


void RegisterDlg::onCancelButtonClicked()
{
    emit switchLoginDlg();
}

void RegisterDlg::onRegisterButtonClicked()
{
    bool valid = checkUserValid();
    if (!valid) {
        return;
    }

    valid = checkEmailValid();
    if (!valid) {
        return;
    }

    valid = checkPassValid();
    if (!valid) {
        return;
    }

    valid = checkConfirmValid();
    if (!valid) {
        return;
    }

    valid = checkVerifyValid();
    if (!valid) {
        return;
    }

    QString strUser = m_ui.lineEdit_user->text();
    QString strEmail = m_ui.lineEdit_email->text();
    QString strPassword = m_ui.lineEdit_password->text();
    QString strCode = m_ui.lineEdit_code->text();

    auto regiserTask = std::make_shared<RegisterTask>(
        m_spClient, strUser.toStdString(),
        strEmail.toStdString(),
        strPassword.toStdString(),
        strCode.toStdString(),
        this,
        std::bind(&RegisterDlg::onRegisterSuccess, this, std::placeholders::_1),
        std::bind(&RegisterDlg::onRegisterError, this, std::placeholders::_1)
    );

    TaskHandler::getInstance().registerNetTask(std::move(regiserTask));

}

void RegisterDlg::onCodeButtonClicked()
{
    bool match = checkEmailValid();
    QString email = m_ui.lineEdit_email->text();
    if (match)
    {
        //发送验证码
        auto verifyCodeTask = std::make_shared<GetVerifyCodeTask>(m_spClient, 
            email.toStdString(),
            this,
            std::bind(&RegisterDlg::onVerifyCodeSuccess,this),
            std::bind(&RegisterDlg::onVerifyCodeError,this,std::placeholders::_1));


        TaskHandler::getInstance().registerNetTask(std::move(verifyCodeTask));
    }
}

void RegisterDlg::onUserTextChanged(const QString& text)
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

void RegisterDlg::onEmailTextChanged(const QString& text)
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

void RegisterDlg::onPasswordTextChanged(const QString& text)
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

void RegisterDlg::onConfirmTextChanged(const QString& text)
{
    QString password = m_ui.lineEdit_password->text();
    if (text.isEmpty()) {
        setLineEditError(m_ui.lineEdit_confirm, true);
        showFieldError("confirm", tr("确认密码不能为空"));
    }
    else if (text != password) {
        setLineEditError(m_ui.lineEdit_confirm, true);
        showFieldError("confirm", tr("两次输入的密码不匹配"));
    }
    else {
        setLineEditError(m_ui.lineEdit_confirm, false);
        hideFieldError("confirm");
    }
}

void RegisterDlg::onCodeTextChanged(const QString& text)
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

bool RegisterDlg::checkUserValid()
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

bool RegisterDlg::checkEmailValid()
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

bool RegisterDlg::checkPassValid()
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

bool RegisterDlg::checkConfirmValid()
{
    QString confirm = m_ui.lineEdit_confirm->text();
    QString password = m_ui.lineEdit_password->text();

    if (confirm.isEmpty()) {
        setLineEditError(m_ui.lineEdit_confirm, true);
        showFieldError("confirm", tr("确认密码不能为空"));
        return false;
    }

    if (confirm != password) {
        setLineEditError(m_ui.lineEdit_confirm, true);
        showFieldError("confirm", tr("两次输入的密码不匹配"));
        return false;
    }

    setLineEditError(m_ui.lineEdit_confirm, false);
    hideFieldError("confirm");
    return true;
}

bool RegisterDlg::checkVerifyValid()
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

void RegisterDlg::switchLabelPassVisible()
{
    auto state = m_ui.label_pass_visible->getCurState();
    if (state == ClickState::Normal)
    {
        m_ui.lineEdit_password->setEchoMode(QLineEdit::Password);
    }
    else
    {
        m_ui.lineEdit_password->setEchoMode(QLineEdit::Normal);
    }
}

void RegisterDlg::switchLabelConfirmVisible()
{
    auto state = m_ui.label_confirm_visible->getCurState();
    if (state == ClickState::Normal)
    {
        m_ui.lineEdit_confirm->setEchoMode(QLineEdit::Password);
    }
    else
    {
        m_ui.lineEdit_confirm->setEchoMode(QLineEdit::Normal);
    }
}

