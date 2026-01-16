#ifndef ZHKEYESIMCLIENT_UI_APPLYFRIENDDIALOG_H_
#define ZHKEYESIMCLIENT_UI_APPLYFRIENDDIALOG_H_


#include <QDialog>
#include <QHash>
#include "ui_ApplyFriendDialog.h"

#include "UI/FriendLabelFrame.h"
#include "UI/global.h"
#include <memory>
#include <memory>

class ClickedLabel;

class ApplyFriendDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyFriendDialog(QWidget *parent = nullptr);
    ~ApplyFriendDialog();
    
    void initTipLabels();
    void addTipLabels(ClickedLabel* label, QPoint curPoint, 
        QPoint& nextPoint, int textWidth, int textHeight);

    bool eventFilter(QObject* watched, QEvent* event);
    void setSearchInfo(std::shared_ptr<SearchInfo> searchInfo);

private:
    void resetLabels();
    void addLabel(const QString& name);

public slots:
    void onLabelMoreClicked();
    void onLineEditLabelEnter();
    void onRemoveFriendLabel(QString);
    void onChangeFriendLabelByTip(QString, ClickLbState);
    void onLineEditLabelTextChange(const QString& text);
    void onLineEditLabelFinished();
    void onLabelTipClickedOnce(QString text);
    void onApplyConfirm();
    void onApplyCancel();

private:
    Ui::ApplyFriendDialogClass ui;

    QHash<QString, ClickedLabel*> m_addLabels;
    std::vector<QString>        m_addLabelKeys;
    QPoint                  m_labelPoint;

    QHash<QString, FriendLabelFrame*> m_friendLabels;
    std::vector<QString>    m_friendLabelKeys;
    std::vector<QString>        m_tipData;
    QPoint                  m_tipCurPoint;

    std::shared_ptr<SearchInfo> m_searchInfo;
};

#endif // !ZHKEYESIMCLIENT_UI_APPLYFRIENDDIALOG_H_