#pragma once

#include <QtWidgets/QDialog>
#include "ui_LoginDlg.h"

class LoginDlg : public QDialog
{
    Q_OBJECT

public:
    LoginDlg(QWidget *parent = nullptr);
    ~LoginDlg();

private:
    Ui::LoginDlgClass ui;
};

