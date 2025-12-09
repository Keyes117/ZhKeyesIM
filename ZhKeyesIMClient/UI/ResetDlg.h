#ifndef UI_RESETDLG_H_
#define UI_RESETDLG_H_
#include <QDialog>
#include "ui_ResetDlg.h"

class ResetDlg : public QDialog
{
    Q_OBJECT

public:
    ResetDlg(QWidget *parent = nullptr);
    ~ResetDlg();

private:
    Ui::ResetDlgClass ui;
};

#endif // UI_RESETDLG_H_

