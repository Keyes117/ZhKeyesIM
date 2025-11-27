#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_loginDlg(new LoginDlg(this)),
    m_registerDlg(new RegisterDlg(this))
{
    m_ui.setupUi(this);

    setCentralWidget(m_loginDlg);

    m_registerDlg->hide();

    connect(m_loginDlg, &LoginDlg::switchRegisterDlg, this, &MainWindow::switchToRegisterDlg);
    connect(m_registerDlg, &RegisterDlg::switchLoginDlg, this, &MainWindow::switchToLoginDlg);

    m_loginDlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    m_registerDlg->setWindowFlags(Qt::CustomizeWindowHint| Qt::FramelessWindowHint);
}

MainWindow::~MainWindow()
{}

void MainWindow::switchToRegisterDlg()
{
    setCentralWidget(m_registerDlg);
    m_loginDlg->hide();

}

void MainWindow::switchToLoginDlg()
{
    setCentralWidget(m_loginDlg);
    m_registerDlg->hide();
}