#include "ApplyFriendDialog.h"

#include <QFontMetrics>
#include <QScrollBar>

#include "Base/UserSession.h"
#include "Base/global.h"
#include "UI/ClickedLabel.h"

ApplyFriendDialog::ApplyFriendDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setModal(this);

    ui.lineEdit_name->setPlaceholderText(tr("ZhKeyes"));
    ui.lineEdit_label->setPlaceholderText("搜索、添加标签");
    ui.lineEdit_back->setPlaceholderText("埃塞克斯");

    ui.lineEdit_back->setMaxLength(21);
    ui.lineEdit_back->move(2, 2);
    ui.lineEdit_back->setMaxLength(10);
    ui.widget_inputTip->hide();

    m_tipCurPoint = QPoint(5, 5);
    //测试数据
    m_tipData = { "同学","家人","菜鸟教程","C++ Primer","Rust 程序设计", "父与子学Python","nodejs开发指南","go 语言开发指南",
                               "游戏伙伴","金融投资","微信读书","拼多多拼友" };


    initTipLabels();

    connect(ui.label_more, &ClickedOnceLabel::clicked, this, &ApplyFriendDialog::onLabelMoreClicked);
    connect(ui.lineEdit_label, &CustomizeEdit::returnPressed, this, &ApplyFriendDialog::onLineEditLabelEnter);
    connect(ui.lineEdit_label, &CustomizeEdit::textChanged, this, &ApplyFriendDialog::onLineEditLabelTextChange);
    connect(ui.lineEdit_label, &CustomizeEdit::editingFinished, this, &ApplyFriendDialog::onLineEditLabelFinished);
    connect(ui.label_tip, &ClickedOnceLabel::clicked, this, &ApplyFriendDialog::onLabelTipClickedOnce);

    ui.scrollArea->horizontalScrollBar()->setHidden(true);
    ui.scrollArea->verticalScrollBar()->setHidden(true);
    ui.scrollArea->installEventFilter(this);
    ui.button_confirm->SetState("normal", "hover", "press");
    ui.button_cancel->SetState("normal", "hover", "press");

    connect(ui.button_cancel, &QPushButton::clicked, this, &ApplyFriendDialog::onApplyCancel);
    connect(ui.button_confirm, &QPushButton::clicked, this, &ApplyFriendDialog::onApplyConfirm);

}

ApplyFriendDialog::~ApplyFriendDialog()
{}

void ApplyFriendDialog::initTipLabels()
{
    int lines = 1;
    for (int i = 0; i < m_tipData.size(); i++)
    {
        auto* label = new ClickedLabel(ui.widget_labelList);
        label->setState("normal", "hover", "pressed", "selected_normal",
            "selected_hover", "selected_pressed");
        label->setObjectName("label_tip");
        label->setText(m_tipData[i]);

        connect(label, &ClickedLabel::clicked, this, &ApplyFriendDialog::onChangeFriendLabelByTip);

        QFontMetrics fontMetrics(label->font());
        int textWidth = fontMetrics.horizontalAdvance(label->text());
        int textHeight = fontMetrics.height();

        if (m_tipCurPoint.x() + textWidth + tip_offset > ui.widget_labelList->width())
        {
            lines++;
            if (lines > 2)
            {
                delete label;
                return;
            }

            m_tipCurPoint.setX(tip_offset);
            m_tipCurPoint.setY(m_tipCurPoint.y() + textHeight + 15);
        }

        auto nextPoint = m_tipCurPoint;

        addTipLabels(label, m_tipCurPoint, nextPoint, textWidth, textHeight);

        m_tipCurPoint = nextPoint;
    }
}

void ApplyFriendDialog::addTipLabels(ClickedLabel* label, QPoint curPoint, QPoint& nextPoint, int textWidth, int textHeight)
{
    label->move(curPoint);
    label->show();

    m_addLabels.insert(label->text(), label);
    m_addLabelKeys.push_back(label->text());
    nextPoint.setX(label->pos().x() + textWidth + 15);
    nextPoint.setY(label->pos().y());
}

bool ApplyFriendDialog::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == ui.scrollArea && event->type() == QEvent::Enter)
    {
        ui.scrollArea->verticalScrollBar()->setHidden(false);
    }
    else if (watched == ui.scrollArea && event->type() == QEvent::Leave)
    {
        ui.scrollArea->verticalScrollBar()->setHidden(true);
    }
    return QObject::eventFilter(watched, event);
}

void ApplyFriendDialog::setSearchInfo(std::shared_ptr<SearchInfo> searchInfo)
{
    m_searchInfo = searchInfo;
    auto applyname = UserSession::getInstance().getUsername();
    auto bakname = searchInfo->m_name;
    ui.lineEdit_name->setText(QString::fromStdString(applyname));
    ui.lineEdit_back->setText(bakname);
}

void ApplyFriendDialog::resetLabels()
{
    auto maxWidth = ui.widget_grid->width();
    auto labelHeight = 0;

    for (auto iter = m_friendLabels.begin(); 
        iter != m_friendLabels.end(); iter++)
    {
        //TODO:  宽度统计

        if (m_labelPoint.x() + iter.value()->width() > maxWidth)
        {
            m_labelPoint.setY(m_labelPoint.y() + iter.value()->height() +6 );
            m_labelPoint.setX(2);
        }

        iter.value()->move(m_labelPoint);
        iter.value()->show();

        m_labelPoint.setX(m_labelPoint.x() + iter.value()->width() + 2);
        m_labelPoint.setY(m_labelPoint.y());
        labelHeight = iter.value()->height();
    }

    if (m_friendLabels.isEmpty())
    {
        ui.lineEdit_label->move(m_labelPoint);
        return;
    }

    if (m_labelPoint.x() + MIN_APPLY_LABEL_ED_LEN > ui.widget_grid->width())
    {
        ui.lineEdit_label->move(2, m_labelPoint.y() + labelHeight + 6);
    }
    else
    {
        ui.lineEdit_back->move(m_labelPoint);
    }
}

void ApplyFriendDialog::addLabel(const QString& name)
{
    if (m_friendLabels.find(name) != m_friendLabels.end())
        return;

    auto label_temp = new FriendLabelFrame(ui.widget_grid);
    label_temp->SetText(name);
    

    auto maxWidth = ui.widget_grid->width();

    if (m_labelPoint.x() + label_temp->width() > maxWidth)
    {
        m_labelPoint.setY(m_labelPoint.x() + label_temp->height() + 6);
        m_labelPoint.setX(2);
    }

    label_temp->move(m_labelPoint);
    label_temp->show();
    m_friendLabels[label_temp->Text()] = label_temp;
    m_friendLabelKeys.push_back(label_temp->Text());

    connect(label_temp, &FriendLabelFrame::close, this, &ApplyFriendDialog::onRemoveFriendLabel);

    m_labelPoint.setX(m_labelPoint.x() + label_temp->width() + 2);

    if (m_labelPoint.x() + MIN_APPLY_LABEL_ED_LEN > ui.widget_grid->width()) {
        ui.lineEdit_label->move(2, m_labelPoint.y() + label_temp->height() + 2);
    }
    else {
        ui.lineEdit_label->move(m_labelPoint);
    }
    ui.lineEdit_label->clear();

    if (ui.widget_grid->height() < m_labelPoint.y() + label_temp->height() + 2) {
        ui.widget_grid->setFixedHeight(m_labelPoint.y() + label_temp->height() * 2 + 2);
    }

}

void ApplyFriendDialog::onLabelMoreClicked()
{
    ui.widget_moreLabel->hide();

    ui.widget_labelList->setFixedWidth(325);
    m_tipCurPoint = QPoint(5, 5);
    auto nextPoint = m_tipCurPoint;

    int textWidth;
    int textHeight;

    //当前所有label 打散重新排列
    for (auto& addedKey : m_addLabelKeys)
    {
        auto addedLabel = m_addLabels[addedKey];

        QFontMetrics fontMetrics(addedLabel->font()); // 获取QLabel控件的字体信息
        textWidth = fontMetrics.horizontalAdvance(addedLabel->text()); // 获取文本的宽度
        textHeight = fontMetrics.height(); // 获取文本的高度

        if (m_tipCurPoint.x() + textWidth + tip_offset > ui.widget_labelList->width()) 
        {
            m_tipCurPoint.setX(tip_offset);
            m_tipCurPoint.setY(m_tipCurPoint.y() + textHeight + 15);
        }
        addedLabel->move(m_tipCurPoint);

        nextPoint.setX(addedLabel->pos().x() + textWidth + 15);
        nextPoint.setY(m_tipCurPoint.y());

        m_tipCurPoint = nextPoint;
    }

    //添加未添加的
    for (int i = 0; i < m_tipData.size(); i++)
    {
        auto iter = m_addLabels.find(m_tipData[i]);
        if (iter != m_addLabels.end())
            continue;

        auto* label = new ClickedLabel(ui.widget_labelList);

        label->setState("normal", "hover", "pressed", "selected_normal",
            "selected_hover", "selected_pressed");

        label->setObjectName("label_tip");
        label->setText(m_tipData[i]);

        connect(label, &ClickedLabel::clicked, this, &ApplyFriendDialog::onChangeFriendLabelByTip);

        QFontMetrics fontMetrics(label->font());
        int textWidth = fontMetrics.horizontalAdvance(label->text());
        int textHeight = fontMetrics.height();

        if (m_tipCurPoint.x() + textWidth + tip_offset > ui.widget_labelList->width())
        {

            m_tipCurPoint.setX(tip_offset);
            m_tipCurPoint.setY(m_tipCurPoint.y() + textHeight + 15);
        }

        nextPoint = m_tipCurPoint;

        addTipLabels(label, m_tipCurPoint, nextPoint, textWidth, textHeight);

        m_tipCurPoint = nextPoint;
    }

    int heightDiff = nextPoint.y() + textHeight + tip_offset - ui.widget_labelGroup->height();
    ui.widget_labelList->setFixedHeight(nextPoint.y() + textHeight + tip_offset);
    ui.scrollcontent->setFixedHeight(ui.scrollcontent->height() + heightDiff);
}

void ApplyFriendDialog::onLineEditLabelEnter()
{
    if (ui.lineEdit_label->text().isEmpty())
        return;

    auto text = ui.lineEdit_label->text();
    addLabel(text);

    ui.widget_inputTip->hide();
    auto find_it = std::find(m_tipData.begin(), m_tipData.end(), text);

    if (find_it == m_tipData.end())
        m_tipData.emplace_back(text);

    auto find_add = m_addLabels.find(text);
    if (find_add != m_addLabels.end())
    {
        find_add.value()->setCurState(ClickLbState::Selected);
        return;
    }

    auto* label = new ClickedLabel(ui.widget_labelList);
    label->setState("normal", "hover", "pressed", "selected_normal",
        "selected_hover", "selected_pressed");
    label->setObjectName("label_tip");
    label->setText(*find_it);

    connect(label, &ClickedLabel::clicked, this, &ApplyFriendDialog::onChangeFriendLabelByTip);

    QFontMetrics fontMetrics(label->font());
    int textWidth = fontMetrics.horizontalAdvance(label->text());
    int textHeight = fontMetrics.height();

    if (m_tipCurPoint.x() + textWidth + tip_offset > ui.widget_labelList->width())
    {

        m_tipCurPoint.setX(tip_offset);
        m_tipCurPoint.setY(m_tipCurPoint.y() + textHeight + 15);
    }

    auto nextPoint = m_tipCurPoint;

    addTipLabels(label, m_tipCurPoint, nextPoint, textWidth, textHeight);

    m_tipCurPoint = nextPoint;


    int heightDiff = nextPoint.y() + textHeight + tip_offset - ui.widget_labelGroup->height();
    ui.widget_labelList->setFixedHeight(nextPoint.y() + textHeight + tip_offset);
    ui.scrollcontent->setFixedHeight(ui.scrollcontent->height() + heightDiff);
    label->setCurState(ClickLbState::Selected);

}

void ApplyFriendDialog::onRemoveFriendLabel(QString name)
{
    m_labelPoint.setX(2);
    m_labelPoint.setY(6);
    auto find_iter = m_friendLabels.find(name);
    if (find_iter == m_friendLabels.end()) {
        return;
    }
    auto find_key = m_friendLabelKeys.end();
    for (auto iter = m_friendLabelKeys.begin(); iter != m_friendLabelKeys.end();
        iter++) {
        if (*iter == name) {
            find_key = iter;
            break;
        }
    }
    if (find_key != m_friendLabelKeys.end()) {
        m_friendLabelKeys.erase(find_key);
    }
    delete find_iter.value();
    m_friendLabels.erase(find_iter);
    resetLabels();
    auto find_add = m_addLabels.find(name);
    if (find_add == m_addLabels.end()) {
        return;
    }
    find_add.value()->resetNormalState();
}

void ApplyFriendDialog::onChangeFriendLabelByTip(QString text, ClickLbState state)
{
    auto find_iter = m_addLabels.find(text);
    if (find_iter == m_addLabels.end()) {
        return;
    }
    if (state == ClickLbState::Selected) {
        //编写添加逻辑
        addLabel(text);
        return;
    }
    if (state == ClickLbState::Normal) {
        //编写删除逻辑
        onRemoveFriendLabel(text);
        return;
    }
}

void ApplyFriendDialog::onLineEditLabelTextChange(const QString& text)
{
    if (text.isEmpty()) {
        ui.label_tip->setText("");
        ui.widget_inputTip->hide();
        return;
    }
    auto iter = std::find(m_tipData.begin(), m_tipData.end(), text);
    if (iter == m_tipData.end()) {
        auto new_text = add_prefix + text;
        ui.label_tip->setText(new_text);
        ui.widget_inputTip->show();
        return;
    }
    ui.label_tip->setText(text);
    ui.widget_inputTip->show();
}

void ApplyFriendDialog::onLineEditLabelFinished()
{
    ui.widget_inputTip->hide();
}

void ApplyFriendDialog::onLabelTipClickedOnce(QString text)
{
    int index = text.indexOf(add_prefix);
    if (index != -1) {
        text = text.mid(index + add_prefix.length());
    }
    addLabel(text);
    auto find_it = std::find(m_tipData.begin(), m_tipData.end(), text);
    //找到了就只需设置状态为选中即可
    if (find_it == m_tipData.end()) 
    {
        m_tipData.push_back(text);
    }
    //判断标签展示栏是否有该标签
    auto find_add = m_addLabels.find(text);
    if (find_add != m_addLabels.end()) 
    {
        find_add.value()->setCurState(ClickLbState::Selected);
        return;
    }
    //标签展示栏也增加一个标签, 并设置绿色选中
    auto* lb = new ClickedLabel(ui.widget_labelList);

    lb->setState("normal", "hover", "pressed", "selected_normal",
        "selected_hover", "selected_pressed");
    lb->setObjectName("label_tip");
    lb->setText(text);
    connect(lb, &ClickedLabel::clicked, this, &ApplyFriendDialog::onChangeFriendLabelByTip);

    QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
    int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
    int textHeight = fontMetrics.height(); // 获取文本的高度

    if (m_tipCurPoint.x() + textWidth + tip_offset + 3 > ui.widget_labelList->width()) {
        m_tipCurPoint.setX(5);
        m_tipCurPoint.setY(m_tipCurPoint.y() + textHeight + 15);
    }
    auto next_point = m_tipCurPoint;

    addTipLabels(lb, m_tipCurPoint, next_point, textWidth, textHeight);
    m_tipCurPoint = next_point;

    int diff_height = next_point.y() + textHeight + tip_offset - ui.widget_labelList->height();
    ui.widget_labelList->setFixedHeight(next_point.y() + textHeight + tip_offset);
    lb->setCurState(ClickLbState::Selected);

    ui.scrollcontent->setFixedHeight(ui.scrollcontent->height() + diff_height);
}

void ApplyFriendDialog::onApplyConfirm()
{
    this->hide();
    deleteLater();
}

void ApplyFriendDialog::onApplyCancel()
{
    this->hide();
    deleteLater();
}

