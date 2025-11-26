#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_loginDlg(new LoginDlg),
    m_registerDlg(new RegisterDlg)
{
    m_ui.setupUi(this);

    setCentralWidget(m_loginDlg);
    m_loginDlg->show();
    m_registerDlg->hide();

    connect(m_loginDlg, &LoginDlg::switchRegisterDlg, this, &MainWindow::showRegisterDlg);
}

MainWindow::~MainWindow()
{}

void MainWindow::switchToRegisterDlg()
{
    setCentralWidget(m_registerDlg);
    m_loginDlg->hide();
    m_registerDlg->show();

}

void MainWindow::switchToLoginDlg()
{
    setCentralWidget(m_loginDlg);
    m_registerDlg->hide();
    m_loginDlg->show();
}