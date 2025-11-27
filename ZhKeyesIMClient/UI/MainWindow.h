#ifndef UI_MAINWINDOW_H_
#define UI_MAINWINDOW_H_

#include <QMainWindow>
#include <QStackedWidget>
#include "ui_MainWindow.h"

#include "LoginDlg.h"
#include "RegisterDlg.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void switchToRegisterDlg();
    void switchToLoginDlg();
    
private:
    Ui::MainWindowClass m_ui;

    QStackedWidget*     m_stackedWidget;
    LoginDlg*           m_loginDlg;
    RegisterDlg*        m_registerDlg;
};

#endif