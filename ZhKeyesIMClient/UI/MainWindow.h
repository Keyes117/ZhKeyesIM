#ifndef ZHKEYESIMCLIENT_UI_MAINWINDOW_H_
#define ZHKEYESIMCLIENT_UI_MAINWINDOW_H_

#include <memory>

#include <QMainWindow>
#include <QStackedWidget>
#include "ui_MainWindow.h"


#include "UI/ChatDialog.h"
#include "UI/LoginDlg.h"
#include "UI/RegisterDlg.h"
#include "UI/ResetDlg.h"

#include "NetWork/IMClient.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void switchToRegisterDlg();
    void switchToResetDlg();
    void switchToLoginDlg();
    void switchToChatDlg();
private:
    Ui::MainWindowClass m_ui;

    QStackedWidget*     m_stackedWidget;
    ChatDialog*         m_chatDlg;
    LoginDlg*           m_loginDlg;
    RegisterDlg*        m_registerDlg;
    ResetDlg*           m_resetDlg;

};

#endif