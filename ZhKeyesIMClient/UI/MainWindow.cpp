#include "MainWindow.h"

MainWindow::MainWindow(std::shared_ptr<IMClient> spClient,QWidget *parent)
    : QMainWindow(parent),
    m_spClient(spClient),
    m_stackedWidget(new QStackedWidget(this)),
    m_loginDlg(new LoginDlg(m_spClient,m_stackedWidget)),
    m_registerDlg(new RegisterDlg(m_spClient,m_stackedWidget))
{
    m_ui.setupUi(this);

    m_registerDlg->hide();

    m_stackedWidget->addWidget(m_loginDlg);
    m_stackedWidget->addWidget(m_registerDlg);
    m_stackedWidget->setCurrentWidget(m_loginDlg);
    setCentralWidget(m_stackedWidget);

    connect(m_loginDlg, &LoginDlg::switchRegisterDlg, this, &MainWindow::switchToRegisterDlg);
    connect(m_registerDlg, &RegisterDlg::switchLoginDlg, this, &MainWindow::switchToLoginDlg);

    m_loginDlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    m_registerDlg->setWindowFlags(Qt::CustomizeWindowHint| Qt::FramelessWindowHint);
}

MainWindow::~MainWindow()
{}

void MainWindow::switchToRegisterDlg()
{
    if (m_stackedWidget)
        m_stackedWidget->setCurrentWidget(m_registerDlg);
}

void MainWindow::switchToLoginDlg()
{
    if (m_stackedWidget)
        m_stackedWidget->setCurrentWidget(m_loginDlg);

}