#ifndef AUTHENFRIEND_H
#define AUTHENFRIEND_H

#include <QDialog>
#include "UI/ClickedLabel.h"
#include "UI/FriendLabelFrame.h"
#include "base/UserData.h"

#include "ui_AuthenFriendDialog.h"

class AuthenFriendDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthenFriendDialog(QWidget *parent = nullptr);
    ~AuthenFriendDialog();

    void InitTipLbs();
    void AddTipLbs(ClickedLabel*, QPoint cur_point, QPoint &next_point, int text_width, int text_height);
    bool eventFilter(QObject *obj, QEvent *event);
    void SetApplyInfo(std::shared_ptr<ApplyInfo> apply_info);
private:
    void resetLabels();

    //已经创建好的标签
    QMap<QString, ClickedLabel*> m_add_labels;
    std::vector<QString> m_add_label_keys;
    QPoint m_label_point;
    //用来在输入框显示添加新好友的标签
    QMap<QString, FriendLabelFrame*> m_friend_labels;
    std::vector<QString> m_friend_label_keys;
    void addLabel(QString name);
    std::vector<QString> m_tip_data;
    QPoint m_tip_cur_point;
public slots:
    //显示更多label标签
    void onMoreLabelClicked();
    //输入label按下回车触发将标签加入展示栏
    void onLineEditLabelEnter();
    //点击关闭，移除展示栏好友便签
    void onFriendLabelCloseClicked(QString);
    //通过点击tip实现增加和减少好友便签
    void onTipClicked(QString, ClickLbState);
    //输入框文本变化显示不同提示
    void onLineEditLabelTextChange(const QString& text);
    //输入框输入完成
    void onLineEditLabelFinished();
   //输入标签显示提示框，点击提示框内容后添加好友便签
    void onAddFirendLabelByClickTip(QString text);
    //处理确认回调
    void onButtonConfirmClicked();
    //处理取消回调
    void onButtonCancelClicked();
   
private:
 	std::shared_ptr<ApplyInfo> m_apply_info;
    Ui::AuthenFriendDialog *m_ui;
};

#endif // AUTHENFRIEND_H
