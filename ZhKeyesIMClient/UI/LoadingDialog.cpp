#include "LoadingDialog.h"


#include <QLabel>
#include <QMovie>
#include <QGraphicsOpacityEffect>
#include <QGuiApplication>
#include <QScreen>

LoadingDialog::LoadingDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);


    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint |
        Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);

    setAttribute(Qt::WA_TranslucentBackground);

    setFixedSize(parent->size());

    QMovie* movie = new QMovie(":/res/res/loading.gif");
    ui.label->setMovie(movie);
    movie->start();
    

}

LoadingDialog::~LoadingDialog()
{}

