#ifndef ZHKEYESIMCLIENT_UI_SEARCHLISTWIDGET_H_
#define ZHKEYESIMCLIENT_UI_SEARCHLISTWIDGET_H_

#include <QListWidget>

#include "Base/UserData.h"

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