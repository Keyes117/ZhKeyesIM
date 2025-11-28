

#include <iostream>

#include <QtWidgets/QApplication>
#include <QFile>
#include <QTextStream>

#include "IMClient.h"
#include "Logger.h"
#include "TaskHandler.h"
#include "MainWindow.h"


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setStyle("fusion");
    QApplication::setFont(QFont("Microsoft YaHei", 9));


    QFile qss(":/style/style/styleSheet.qss");
    if (qss.open(QFile::ReadOnly | QFile::Text))
    {
        QString styleSheet = QTextStream(&qss).readAll();
        qss.close();
        app.setStyleSheet(styleSheet);
    }
    else
    {
        qWarning() << "Failed to load stylesheet: " << qss.errorString();
    }

#ifdef _DEBUG
    Logger::instance().setLogLevel(LogLevel::DEBUG);
#else
    Logger::instance().setLogLevel(LogLevel::INFO);
#endif
    //Logger::instance().setLogFile("GateServer.log");

    if (!TaskHandler::getInstance().init())
    {
        LOG_ERROR("基础组件初始化失败....");
        return 1;
    }

    std::shared_ptr<IMClient> spIMClient = std::make_shared<IMClient>();
    if (!spIMClient->init("127.0.0.1", 8080, IOMultiplexType::Select))
    {
        LOG_ERROR("网络客户端初始化失败....");
        return 1;
    }       

    if (!spIMClient->connect())
    {
        LOG_ERROR("网络连接失败，请检查网络....");
        return 1;
    }

    MainWindow mainWindow(spIMClient);
    mainWindow.setBaseSize(300, 500);
    mainWindow.setMaximumSize(300, 500);
    mainWindow.show();
    int ret =  app.exec();

    TaskHandler::getInstance().close();

    return ret;
}