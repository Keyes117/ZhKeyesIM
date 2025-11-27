#ifndef UI_REGISTERDLG_H_
#define UI_REGISTERDLG_H_

#include <QDialog>
#include "ui_RegisterDlg.h"

class RegisterDlg : public QDialog
{
    Q_OBJECT

public:
    RegisterDlg(QWidget* parent = nullptr);
    ~RegisterDlg();

private:
    void setUpSignals();
    void showTip(const QString& str, bool isError);

signals:
    void switchLoginDlg();

private slots:
    void onCancelButtonClicked();
    void onRegisterButtonClicked();
    void onCodeButtonClicked();

private:

    Ui::RegisterDlgClass m_ui;
};

#endif