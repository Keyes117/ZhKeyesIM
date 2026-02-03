#include "MainWindow.h"

#include <QMessageBox>


#include "Task/TaskHandler.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_stackedWidget(new QStackedWidget(this)),
    m_chatDlg(new ChatDialog( m_stackedWidget)),
    m_loginDlg(new LoginDlg(m_stackedWidget)),
    m_registerDlg(new RegisterDlg(m_stackedWidget)),
    m_resetDlg(new ResetDlg(m_stackedWidget))
{
    m_ui.setupUi(this);

    m_loginDlg->setWindowFlags(Qt::Widget); 
    m_registerDlg->setWindowFlags(Qt::Widget);
    m_resetDlg->setWindowFlags(Qt::Widget);
    m_chatDlg->setWindowFlags(Qt::Widget);

    m_stackedWidget->addWidget(m_chatDlg);
    m_stackedWidget->addWidget(m_loginDlg);
    m_stackedWidget->addWidget(m_registerDlg);
    m_stackedWidget->addWidget(m_resetDlg);


    //m_stackedWidget->setCurrentWidget(m_chatDlg);
    m_stackedWidget->setCurrentWidget(m_loginDlg);
    setCentralWidget(m_stackedWidget);

    connect(m_loginDlg, &LoginDlg::switchRegisterDlg, this, &MainWindow::switchToRegisterDlg);
    connect(m_loginDlg, &LoginDlg::switchResetDlg, this, &MainWindow::switchToResetDlg);
    connect(m_loginDlg, &LoginDlg::loginSuccess, this, &MainWindow::switchToChatDlg);
    connect(m_registerDlg, &RegisterDlg::switchLoginDlg, this, &MainWindow::switchToLoginDlg);
    connect(m_resetDlg, &ResetDlg::switchLoginDlg, this, &MainWindow::switchToLoginDlg);

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

void MainWindow::switchToChatDlg()
{
    if (m_stackedWidget)
        m_stackedWidget->setCurrentWidget(m_chatDlg);
}
