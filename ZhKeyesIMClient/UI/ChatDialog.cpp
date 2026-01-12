#include "ChatDialog.h"

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    //ui.add_btn->SetState("normal", "hover", "press");
}

ChatDialog::~ChatDialog()
{}

