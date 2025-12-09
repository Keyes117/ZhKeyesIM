#ifndef UI_MAINWINDOW_H_
#define UI_MAINWINDOW_H_

#include <memory>

#include <QMainWindow>
#include <QStackedWidget>
#include "ui_MainWindow.h"


#include "LoginDlg.h"
#include "RegisterDlg.h"
#include "ResetDlg.h"
#include "IMClient.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(std::shared_ptr<IMClient> spClient, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void switchToRegisterDlg();
    void switchToResetDlg();
    void switchToLoginDlg();

    void onErrorMsg(QString errorMsg);
    void onSuccessMsg(QString successMsg);
private:
    Ui::MainWindowClass m_ui;

    std::shared_ptr<IMClient> m_spClient;

    QStackedWidget*     m_stackedWidget;
    LoginDlg*           m_loginDlg;
    RegisterDlg*        m_registerDlg;
    ResetDlg*           m_resetDlg;
};

#endif