

#include <iostream>

#include <QtWidgets/QApplication>
#include <QFile>
#include <QTextStream>

#include "IMClient.h"
#include "Logger.h"
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

    MainWindow mainWindow;
    mainWindow.setBaseSize(300, 500);
    mainWindow.setMaximumSize(300, 500);
    mainWindow.show();
    return app.exec();
}