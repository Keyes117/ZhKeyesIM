#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_ui.setupUi(this);

    m_loginDlg = new LoginDlg(this);
    setCentralWidget(m_loginDlg);
    m_loginDlg->show();
}

MainWindow::~MainWindow()
{}

