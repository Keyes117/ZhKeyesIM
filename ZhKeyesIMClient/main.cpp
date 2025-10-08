

#include <iostream>

#include <QtWidgets/QApplication>

#include "IMClient.h"
#include "Logger.h"
#include "LoginDlg.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setStyle("fusion");
    QApplication::setFont(QFont("Microsoft YaHei", 9));

    Logger::instance().setLogLevel(LogLevel::INFO);

    Logger::instance().setLogFile("client.log");

    LoginDlg loginDlg;
    if (loginDlg.exec() == QDialog::Accepted)
    {
        LOG_INFO("LoginDlg Success");
        return 1;
    }
    return 1;
}