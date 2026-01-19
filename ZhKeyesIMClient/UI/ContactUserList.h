#ifndef ZHKEYESIMCLIENT_UI_CONTACTUSERLISTWIDGET_H_
#define ZHKEYESIMCLIENT_UI_CONTACTUSERLISTWIDGET_H_

#include <QListWidget>


class ContactUserItem;
class ContactUserListWidget : public QListWidget
{
    Q_OBJECT
public:
    ContactUserListWidget(QWidget* parent = nullptr);
    virtual ~ContactUserListWidget() = default;

    void showRedPoint(bool bShow = true);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void addContactUserList();

public slots:
    void onItemClicked(QListWidgetItem* item);

signals:
    void loadingContactUser();
    void switchApplyFriendPage();
    void switchFriendInfoPage();

private:
    ContactUserItem* m_addFriendItem;
    QListWidgetItem* m_groupItem;

};

#endif