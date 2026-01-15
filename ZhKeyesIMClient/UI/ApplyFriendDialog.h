#ifndef ZHKEYESIMCLIENT_UI_APPLYFRIENDDIALOG_H_
#define ZHKEYESIMCLIENT_UI_APPLYFRIENDDIALOG_H_


#include <QDialog>
#include "ui_ApplyFriendDialog.h"

#include "UI/global.h"

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
    void onShowMoreLabel();
    void onLabelEnter();
    void onRemoveFriendLabel(QString);
    void onChangeFriendLabelByTip(QString, ClickLbState);
    void onLabelTextChange(const QString& text);
    void onLabelEditFinished();
    void onAddFriendLabelByClickTip(QString text);
    void onApplySure();
    void onApplyCancel();

private:
    Ui::ApplyFriendDialogClass ui;

    std::unordered_map<QString, ClickedLabel*> m_addLabels;
    std::vector<QString>        m_addLabelKeys;
    QPoint                  m_labelPoint;

    //std::unordered_map<QString, FriendLabel*> m_firendLabels;
    std::vector<QString>    m_friendLabelKeys;
    std::vector<QString>        m_tipData;
};

#endif // !ZHKEYESIMCLIENT_UI_APPLYFRIENDDIALOG_H_