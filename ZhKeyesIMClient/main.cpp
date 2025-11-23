

#include <iostream>

#include <QtWidgets/QApplication>

#include "IMClient.h"
#include "Logger.h"
#include "MainWindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setStyle("fusion");
    QApplication::setFont(QFont("Microsoft YaHei", 9));

#ifdef _DEBUG
    Logger::instance().setLogLevel(LogLevel::DEBUG);
#else
    Logger::instance().setLogLevel(LogLevel::INFO);
#endif
    //Logger::instance().setLogFile("GateServer.log");

    MainWindow mainWindow;
    mainWindow.show();
    return 1;
}