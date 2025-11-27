#include "RegisterDlg.h"

RegisterDlg::RegisterDlg(QWidget* parent)
    : QDialog(parent)
{
    m_ui.setupUi(this);
}

RegisterDlg::~RegisterDlg()
{
}

void RegisterDlg::setUpSignals()
{
    connect(m_ui.button_cancel, &QPushButton::clicked, this,&RegisterDlg::onCancelButtonClicked);
    connect(m_ui.button_register, &QPushButton::clicked, this, &RegisterDlg::onRegisterButtonClicked);
}


void RegisterDlg::onCancelButtonClicked()
{
    emit switchLoginDlg();
}
void RegisterDlg::onRegisterButtonClicked()
{

}
