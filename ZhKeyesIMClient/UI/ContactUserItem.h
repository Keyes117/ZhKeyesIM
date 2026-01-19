#pragma once

#include <QWidget>
#include "ui_ContactUserItem.h"

class ContactUserItem : public QWidget
{
    Q_OBJECT

public:
    ContactUserItem(QWidget *parent = nullptr);
    ~ContactUserItem();

private:
    Ui::ContactUserItemClass ui;
};

