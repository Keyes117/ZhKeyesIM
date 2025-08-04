

#include <iostream>

#include <QtWidgets/QApplication>

#include "IMClient.h"
#include "Logger.h"
#include "LoginDlg.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Logger::instance().setLogLevel(LogLevel::INFO);

    Logger::instance().setLogFile("client.log");

    IMClient client;
    client.init("127.0.0.1", 9000);
    if (!client.connect())
    {
        return 0;
    }

    LoginDlg loginDlg;
    if(loginDlg.exec() == QDialog::Accepted)
    {
        LOG_INFO("LoginDlg Success");
        return 1;
    }


    return 1;



}