#ifndef ZHKEYESIMCLIENT_UI_APPLYFRIENDLISTWIDGET
#define ZHKEYESIMCLIENT_UI_APPLYFRIENDLISTWIDGET

#include <QListWidget>

class ApplyFriendListWidget : public QListWidget
{
    Q_OBJECT
public:
    ApplyFriendListWidget(QWidget* parent = nullptr);

protected:
    virtual bool eventFilter(QObject* watched, QEvent* event) override;

signals:
    void showSearch(bool);
};

#endif