#ifndef ZHKEYESIMCLIENT_UI_FRIENDLABELFRAME_H_
#define ZHKEYESIMCLIENT_UI_FRIENDLABELFRAME_H_

#include <QFrame>
#include "ui_FriendLabelFrame.h"

class FriendLabelFrame : public QFrame
{
    Q_OBJECT

public:
    explicit FriendLabelFrame(QWidget *parent = nullptr);
    ~FriendLabelFrame();


    void SetText(QString text);
    int Width();
    int Height();
    QString Text();

public slots:
    void onLabelCloseClicked();

signals:
    void close(QString);
private:
    Ui::FriendLabelFrameClass ui;

    QString m_text;
    int m_width;
    int m_height;
};

#endif  