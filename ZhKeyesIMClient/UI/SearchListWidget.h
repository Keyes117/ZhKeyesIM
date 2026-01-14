#ifndef ZHKEYESIMCLIENT_UI_SEARCHLISTWIDGET_H_
#define ZHKEYESIMCLIENT_UI_SEARCHLISTWIDGET_H_

#include <QListWidget>

class SearchInfo {
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex):
        m_uid(uid),m_name(name),m_nick(nick),m_desc(desc),m_sex(sex)
    {

    }
    
    int     m_uid;
    QString m_name;
    QString m_nick;
    QString m_desc;
    int     m_sex;
};

class LoadingDialog;
class SearchListWidget : public QListWidget
{
    Q_OBJECT

public:
    SearchListWidget(QWidget* parent = nullptr);
    void closeFindDlg();
    void setSearchEdit(QWidget* eidt);

protected:
    bool eventFilter(QObject* watched, QEvent* event);

private:
    void waitPending(bool pending = true);
    void addTipItem();

private slots:
    void onItemClicked(QListWidgetItem* item);
    void onUserSearch(std::shared_ptr<SearchInfo> info);

private:
    bool m_sendPending = false;
    std::shared_ptr<QDialog> m_findDialog;
    QWidget* m_searchEdit = nullptr;
    LoadingDialog* m_loadingDialog = nullptr;




};

#endif