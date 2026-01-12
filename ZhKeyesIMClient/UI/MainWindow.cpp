#include "MainWindow.h"

#include <QMessageBox>


#include "Task/TaskHandler.h"

MainWindow::MainWindow(std::shared_ptr<IMClient> spClient,QWidget *parent)
    : QMainWindow(parent),
    m_spClient(spClient),
    m_stackedWidget(new QStackedWidget(this)),
    m_chatDlg(new ChatDialog(m_spClient, m_stackedWidget)),
    m_loginDlg(new LoginDlg(m_spClient,m_stackedWidget)),
    m_registerDlg(new RegisterDlg(m_spClient,m_stackedWidget)),
    m_resetDlg(new ResetDlg(m_spClient,m_stackedWidget))
{
    m_ui.setupUi(this);

    m_loginDlg->setWindowFlags(Qt::Widget);  // 而不是 Qt::Dialog
    m_registerDlg->setWindowFlags(Qt::Widget);
    m_resetDlg->setWindowFlags(Qt::Widget);
    m_chatDlg->setWindowFlags(Qt::Widget);

    m_stackedWidget->addWidget(m_chatDlg);
    m_stackedWidget->addWidget(m_loginDlg);
    m_stackedWidget->addWidget(m_registerDlg);
    m_stackedWidget->addWidget(m_resetDlg);


    m_stackedWidget->setCurrentWidget(m_chatDlg);
    //m_stackedWidget->setCurrentWidget(m_loginDlg);
    setCentralWidget(m_stackedWidget);

    connect(m_loginDlg, &LoginDlg::switchRegisterDlg, this, &MainWindow::switchToRegisterDlg);
    connect(m_loginDlg, &LoginDlg::switchResetDlg, this, &MainWindow::switchToResetDlg);
    connect(m_registerDlg, &RegisterDlg::switchLoginDlg, this, &MainWindow::switchToLoginDlg);
    connect(m_resetDlg, &ResetDlg::switchLoginDlg, this, &MainWindow::switchToLoginDlg);

    connect(&TaskHandler::getInstance(), &TaskHandler::reportErrorMsg, this, &MainWindow::onErrorMsg);
    connect(&TaskHandler::getInstance(), &TaskHandler::reportSuccessMsg, this, &MainWindow::onSuccessMsg);

}

MainWindow::~MainWindow()
{}

void MainWindow::switchToRegisterDlg()
{
    if (m_stackedWidget)
        m_stackedWidget->setCurrentWidget(m_registerDlg);
}

void MainWindow::switchToResetDlg()
{
    if (m_stackedWidget)
        m_stackedWidget->setCurrentWidget(m_resetDlg);
}

void MainWindow::switchToLoginDlg()
{
    if (m_stackedWidget)
        m_stackedWidget->setCurrentWidget(m_loginDlg);

}

void MainWindow::onErrorMsg(QString errorMsg)
{
    QMessageBox::critical(this, "错误", errorMsg,"确认");
}

void MainWindow::onSuccessMsg(QString successMsg)
{
    QMessageBox::information(this, "成功", successMsg, "确认");
}
