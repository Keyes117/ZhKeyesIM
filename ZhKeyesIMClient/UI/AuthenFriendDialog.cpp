#include "UI/AuthenFriendDialog.h"

#include "ui_AuthenFriendDialog.h"
#include "UI/ClickedLabel.h"
#include "UI/FriendLabelFrame.h"

#include <QScrollBar>
#include <QPushButton>
#include <QFontMetrics>


AuthenFriendDialog::AuthenFriendDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::AuthenFriendDialog),m_label_point(2,6)
{
    m_ui->setupUi(this);
    // 隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setObjectName("AuthenFriendDialog");
    this->setModal(true);
    m_ui->lineEdit_label->setPlaceholderText("搜索、添加标签");
    m_ui->lineEdit_back->setPlaceholderText("燃烧的胸毛");

    m_ui->lineEdit_label->setMaxLength(21);
    m_ui->lineEdit_label->move(2, 2);
    m_ui->lineEdit_label->setFixedHeight(20);
    m_ui->lineEdit_label->setMaxLength(10);
    m_ui->input_tip_wid->hide();

    m_tip_cur_point = QPoint(5, 5);

    m_tip_data = { "同学","家人","菜鸟教程","C++ Primer","Rust 程序设计",
                             "父与子学Python","nodejs开发指南","go 语言开发指南",
                                "游戏伙伴","金融投资","微信读书","拼多多拼友" };

    connect(m_ui->label_more, &ClickedOnceLabel::clicked, this, &AuthenFriendDialog::onMoreLabelClicked);
    InitTipLbs();
    //链接输入标签回车事件
    connect(m_ui->lineEdit_label, &CustomizeEdit::returnPressed, this, &AuthenFriendDialog::onLineEditLabelEnter);
    connect(m_ui->lineEdit_label, &CustomizeEdit::textChanged, this, &AuthenFriendDialog::onLineEditLabelTextChange);
    connect(m_ui->lineEdit_label, &CustomizeEdit::editingFinished, this, &AuthenFriendDialog::onLineEditLabelFinished);
    connect(m_ui->label_tip, &ClickedOnceLabel::clicked, this, &AuthenFriendDialog::onAddFirendLabelByClickTip);

    m_ui->scrollArea->horizontalScrollBar()->setHidden(true);
    m_ui->scrollArea->verticalScrollBar()->setHidden(true);
    m_ui->scrollArea->installEventFilter(this);
    m_ui->button_confirm->SetState("normal","hover","press");
    m_ui->button_cancel->SetState("normal","hover","press");
    //连接确认和取消按钮的槽函数
    connect(m_ui->button_cancel, &QPushButton::clicked, this, &AuthenFriendDialog::onButtonCancelClicked);
    connect(m_ui->button_confirm, &QPushButton::clicked, this, &AuthenFriendDialog::onButtonConfirmClicked);
}

AuthenFriendDialog::~AuthenFriendDialog()
{
    qDebug()<< "AuthenFriendDialog destruct";
    delete m_ui;
}

void AuthenFriendDialog::InitTipLbs()
{
    int lines = 1;
    for(int i = 0; i < m_tip_data.size(); i++){

        auto* lb = new ClickedLabel(m_ui->widget_labelList);
        lb->setState("normal", "hover", "pressed", "selected_normal",
            "selected_hover", "selected_pressed");
        lb->setObjectName("tipslb");
        lb->setText(m_tip_data[i]);
        connect(lb, &ClickedLabel::clicked, this, &AuthenFriendDialog::onTipClicked);

        QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
        int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
        int textHeight = fontMetrics.height(); // 获取文本的高度

        if (m_tip_cur_point.x() + textWidth + tip_offset > m_ui->widget_labelList->width()) {
            lines++;
            if (lines > 2) {
                delete lb;
                return;
            }

            m_tip_cur_point.setX(tip_offset);
            m_tip_cur_point.setY(m_tip_cur_point.y() + textHeight + 15);

        }

       auto next_point = m_tip_cur_point;

       AddTipLbs(lb, m_tip_cur_point,next_point, textWidth, textHeight);

       m_tip_cur_point = next_point;
    }

}

void AuthenFriendDialog::AddTipLbs(ClickedLabel* lb, QPoint cur_point, QPoint& next_point, int text_width, int text_height)
{
    lb->move(cur_point);
    lb->show();
    m_add_labels.insert(lb->text(), lb);
    m_add_label_keys.push_back(lb->text());
    next_point.setX(lb->pos().x() + text_width + 15);
    next_point.setY(lb->pos().y());
}

bool AuthenFriendDialog::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_ui->scrollArea && event->type() == QEvent::Enter)
    {
        m_ui->scrollArea->verticalScrollBar()->setHidden(false);
    }
    else if (obj == m_ui->scrollArea && event->type() == QEvent::Leave)
    {
        m_ui->scrollArea->verticalScrollBar()->setHidden(true);
    }
    return QObject::eventFilter(obj, event);
}

void AuthenFriendDialog::SetApplyInfo(std::shared_ptr<ApplyInfo> apply_info)
{
    m_apply_info = apply_info;
    m_ui->lineEdit_back->setPlaceholderText(apply_info->_name);
}

void AuthenFriendDialog::onMoreLabelClicked()
{
    qDebug()<< "receive more label clicked";
    m_ui->widget_moreLabel->hide();

    m_ui->widget_labelList->setFixedWidth(325);
    m_tip_cur_point = QPoint(5, 5);
    auto next_point = m_tip_cur_point;
    int textWidth;
    int textHeight;
    //重拍现有的label
    for(auto & added_key : m_add_label_keys){
        auto added_lb = m_add_labels[added_key];

        QFontMetrics fontMetrics(added_lb->font()); // 获取QLabel控件的字体信息
        textWidth = fontMetrics.horizontalAdvance(added_lb->text()); // 获取文本的宽度
        textHeight = fontMetrics.height(); // 获取文本的高度

        if(m_tip_cur_point.x() +textWidth + tip_offset > m_ui->widget_labelList->width()){
            m_tip_cur_point.setX(tip_offset);
            m_tip_cur_point.setY(m_tip_cur_point.y()+textHeight+15);
        }
        added_lb->move(m_tip_cur_point);

        next_point.setX(added_lb->pos().x() + textWidth + 15);
        next_point.setY(m_tip_cur_point.y());

        m_tip_cur_point = next_point;

    }

    //添加未添加的
    for(int i = 0; i < m_tip_data.size(); i++){
        auto iter = m_add_labels.find(m_tip_data[i]);
        if(iter != m_add_labels.end()){
            continue;
        }

        auto* lb = new ClickedLabel(m_ui->widget_labelList);
        lb->setState("normal", "hover", "pressed", "selected_normal",
            "selected_hover", "selected_pressed");
        lb->setObjectName("tipslb");
        lb->setText(m_tip_data[i]);
        connect(lb, &ClickedLabel::clicked, this, &AuthenFriendDialog::onTipClicked);

        QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
        int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
        int textHeight = fontMetrics.height(); // 获取文本的高度

        if (m_tip_cur_point.x() + textWidth + tip_offset > m_ui->widget_labelList->width()) {

            m_tip_cur_point.setX(tip_offset);
            m_tip_cur_point.setY(m_tip_cur_point.y() + textHeight + 15);

        }

         next_point = m_tip_cur_point;

        AddTipLbs(lb, m_tip_cur_point, next_point, textWidth, textHeight);

        m_tip_cur_point = next_point;

    }

   int diff_height = next_point.y() + textHeight + tip_offset - m_ui->widget_labelList->height();
   m_ui->widget_labelList->setFixedHeight(next_point.y() + textHeight + tip_offset);

    //qDebug()<<"after resize ui->lb_list size is " <<  ui->lb_list->size();
    m_ui->scrollcontent->setFixedHeight(m_ui->scrollcontent->height()+diff_height);
}

void AuthenFriendDialog::resetLabels()
{
    auto max_width = m_ui->gridWidget->width();
    auto label_height = 0;
    for(auto iter = m_friend_labels.begin(); iter != m_friend_labels.end(); iter++){
        //todo... 添加宽度统计
        if( m_label_point.x() + iter.value()->width() > max_width) {
            m_label_point.setY(m_label_point.y()+iter.value()->height()+6);
            m_label_point.setX(2);
        }

        iter.value()->move(m_label_point);
        iter.value()->show();

        m_label_point.setX(m_label_point.x()+iter.value()->width()+2);
        m_label_point.setY(m_label_point.y());
        label_height = iter.value()->height();
    }

    if(m_friend_labels.isEmpty()){
         m_ui->lineEdit_label->move(m_label_point);
         return;
    }

    if(m_label_point.x() + MIN_APPLY_LABEL_ED_LEN > m_ui->gridWidget->width()){
        m_ui->lineEdit_label->move(2,m_label_point.y()+label_height+6);
    }else{
         m_ui->lineEdit_label->move(m_label_point);
    }
}

void AuthenFriendDialog::addLabel(QString name)
{
    if (m_friend_labels.find(name) != m_friend_labels.end()) {
        return;
    }

    auto tmplabel = new FriendLabelFrame(m_ui->gridWidget);
    tmplabel->SetText(name);
    tmplabel->setObjectName("FriendLabel");

    auto max_width = m_ui->gridWidget->width();
    //todo... 添加宽度统计
    if (m_label_point.x() + tmplabel->width() > max_width) {
        m_label_point.setY(m_label_point.y() + tmplabel->height() + 6);
        m_label_point.setX(2);
    }
    else {

    }


    tmplabel->move(m_label_point);
    tmplabel->show();
    m_friend_labels[tmplabel->Text()] = tmplabel;
    m_friend_label_keys.push_back(tmplabel->Text());

    connect(tmplabel, &FriendLabelFrame::close, this, &AuthenFriendDialog::onFriendLabelCloseClicked);

    m_label_point.setX(m_label_point.x() + tmplabel->width() + 2);

    if (m_label_point.x() + MIN_APPLY_LABEL_ED_LEN > m_ui->gridWidget->width()) {
        m_ui->lineEdit_label->move(2, m_label_point.y() + tmplabel->height() + 2);
    }
    else {
        m_ui->lineEdit_label->move(m_label_point);
    }

    m_ui->lineEdit_label->clear();

    if (m_ui->gridWidget->height() < m_label_point.y() + tmplabel->height() + 2) {
        m_ui->gridWidget->setFixedHeight(m_label_point.y() + tmplabel->height() * 2 + 2);
    }
}

void AuthenFriendDialog::onLineEditLabelEnter()
{
    if(m_ui->lineEdit_label->text().isEmpty()){
        return;
    }

    addLabel(m_ui->lineEdit_label->text());

    m_ui->input_tip_wid->hide();
}

void AuthenFriendDialog::onFriendLabelCloseClicked(QString name)
{
    qDebug() << "receive close signal";

    m_label_point.setX(2);
    m_label_point.setY(6);

   auto find_iter = m_friend_labels.find(name);

   if(find_iter == m_friend_labels.end()){
       return;
   }

   auto find_key = m_friend_label_keys.end();
   for(auto iter = m_friend_label_keys.begin(); iter != m_friend_label_keys.end();
       iter++){
       if(*iter == name){
           find_key = iter;
           break;
       }
   }

   if(find_key != m_friend_label_keys.end()){
      m_friend_label_keys.erase(find_key);
   }


   delete find_iter.value();

   m_friend_labels.erase(find_iter);

   resetLabels();

   auto find_add = m_add_labels.find(name);
   if(find_add == m_add_labels.end()){
        return;
   }

   find_add.value()->resetNormalState();
}

//点击标已有签添加或删除新联系人的标签
void AuthenFriendDialog::onTipClicked(QString lbtext, ClickLbState state)
{
    auto find_iter = m_add_labels.find(lbtext);
    if(find_iter == m_add_labels.end()){
        return;
    }

    if(state == ClickLbState::Selected){
        //编写添加逻辑
        addLabel(lbtext);
        return;
    }

    if(state == ClickLbState::Normal){
        //编写删除逻辑
        onFriendLabelCloseClicked(lbtext);
        return;
    }

}

void AuthenFriendDialog::onLineEditLabelTextChange(const QString& text)
{
    if (text.isEmpty()) {
        m_ui->label_tip->setText("");
        m_ui->input_tip_wid->hide();
        return;
    }

    auto iter = std::find(m_tip_data.begin(), m_tip_data.end(), text);
    if (iter == m_tip_data.end()) {
        auto new_text = add_prefix + text;
        m_ui->label_tip->setText(new_text);
        m_ui->input_tip_wid->show();
        return;
    }
    m_ui->label_tip->setText(text);
    m_ui->input_tip_wid->show();
}

void AuthenFriendDialog::onLineEditLabelFinished()
{
    m_ui->input_tip_wid->hide();
}

void AuthenFriendDialog::onAddFirendLabelByClickTip(QString text)
{
    int index = text.indexOf(add_prefix);
    if (index != -1) {
        text = text.mid(index + add_prefix.length());
    }
    addLabel(text);
    //标签展示栏也增加一个标签, 并设置绿色选中
    if (index != -1) {
        m_tip_data.push_back(text);
    }

    auto* lb = new ClickedLabel(m_ui->widget_labelList);
    lb->setState("normal", "hover", "pressed", "selected_normal",
        "selected_hover", "selected_pressed");
    lb->setObjectName("tipslb");
    lb->setText(text);
    connect(lb, &ClickedLabel::clicked, this, &AuthenFriendDialog::onTipClicked);
    qDebug() << "ui->lb_list->width() is " << m_ui->widget_labelList->width();
    qDebug() << "_tip_cur_point.x() is " << m_tip_cur_point.x();

    QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
    int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
    int textHeight = fontMetrics.height(); // 获取文本的高度
    qDebug() << "textWidth is " << textWidth;

    if (m_tip_cur_point.x() + textWidth+ tip_offset+3 > m_ui->widget_labelList->width()) {

        m_tip_cur_point.setX(5);
        m_tip_cur_point.setY(m_tip_cur_point.y() + textHeight + 15);

    }

    auto next_point = m_tip_cur_point;

     AddTipLbs(lb, m_tip_cur_point, next_point, textWidth,textHeight);
    m_tip_cur_point = next_point;

    int diff_height = next_point.y() + textHeight + tip_offset - m_ui->widget_labelList->height();
    m_ui->widget_labelList->setFixedHeight(next_point.y() + textHeight + tip_offset);

    lb->setCurState(ClickLbState::Selected);

    m_ui->scrollcontent->setFixedHeight(m_ui->scrollcontent->height()+ diff_height );
}

void AuthenFriendDialog::onButtonConfirmClicked()
{
    qDebug() << "Slot Apply Sure ";
    //添加发送逻辑
    //QJsonObject jsonObj;
    //auto uid = UserMgr::GetInstance()->GetUid();
    //jsonObj["fromuid"] = uid;
    //jsonObj["touid"] = _apply_info->_uid;
    //QString back_name = "";
    //if(ui->back_ed->text().isEmpty()){
    //    back_name = ui->back_ed->placeholderText();
    //}else{
    //    back_name = ui->back_ed->text();
    //}
    //jsonObj["back"] = back_name;

    //QJsonDocument doc(jsonObj);
    //QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    ////发送tcp请求给chat server
    //emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_AUTH_FRIEND_REQ, jsonData);

    this->hide();
    deleteLater();
}

void AuthenFriendDialog::onButtonCancelClicked()
{
    this->hide();
    deleteLater();
}
