#ifndef UI_REGISTERDLG_H_
#define UI_REGISTERDLG_H_

#include <QDialog>

#include "ui_RegisterDlg.h"

#include "IMClient.h"

class RegisterDlg : public QDialog
{
    Q_OBJECT

public:
    RegisterDlg(std::shared_ptr<IMClient> spClient, QWidget* parent = nullptr);
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

    std::shared_ptr<IMClient> m_spClient;
};

#endif