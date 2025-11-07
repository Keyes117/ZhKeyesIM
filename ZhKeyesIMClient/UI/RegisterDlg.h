#pragma once

#include <QDialog>
#include "ui_RegisterDlg.h"

class RegisterDlg : public QDialog
{
    Q_OBJECT

public:
    RegisterDlg(QWidget* parent = nullptr);
    ~RegisterDlg();

private:
    Ui::RegisterDlgClass m_ui;
};

