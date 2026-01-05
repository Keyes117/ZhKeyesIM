

#include <iostream>

#include <QtWidgets/QApplication>
#include <QFile>
#include <QTextStream>

#include "NetWork/IMClient.h"
#include "Logger.h"
#include "Task/TaskHandler.h"
#include "UI/MainWindow.h"
#include "common.h"


int main(int argc, char* argv[])
{
#ifdef _WIN32
    if (!net::SocketUtil::InitNetwork())
    {
        std::cerr << "Failed to initialize network library!" << std::endl;
        return 1;
    }
#endif

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
    Logger::instance().setLogFile("IMClient.log");

    ZhKeyes::Util::ConfigManager config;
    if (!config.load("config.json"))
    {
        LOG_ERROR("配置文件读取失败....");
        return 1;
    }

    std::shared_ptr<IMClient> spIMClient = std::make_shared<IMClient>();
    if (!spIMClient->init(config))
    {
        LOG_ERROR("网络客户端初始化失败....");
        return 1;
    }       

    if (!TaskHandler::getInstance().init())
    {
        LOG_ERROR("基础组件初始化失败....");
        return 1;
    }

    MainWindow mainWindow(spIMClient);
    mainWindow.setBaseSize(300, 500);
    mainWindow.setMaximumSize(300, 500);
    mainWindow.show();
    int ret =  app.exec();

    TaskHandler::getInstance().close();

#ifdef _WIN32
    net::SocketUtil::CleanupNetwork();
#endif


    return ret;
}