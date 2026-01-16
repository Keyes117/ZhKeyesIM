#include "FindSuccessDialog.h"

#include <QDir>


#include "UI/ClickedButton.h"

FindSuccessDialog::FindSuccessDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    setWindowTitle("添加");

    //setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    QString appPath = QCoreApplication::applicationDirPath();
    QString pixPath = QDir::toNativeSeparators(appPath +
        QDir::separator() + "static" + QDir::separator() + "head_1.jpg");

    QPixmap headPix(pixPath);
    headPix = headPix.scaled(ui.label_head->size(),
        Qt::KeepAspectRatio, Qt::SmoothTransformation);

    ui.label_head->setPixmap(headPix);
    ui.button_addFriend->SetState("normal", "hover", "press");
    this->setModal(true);
}

FindSuccessDialog::~FindSuccessDialog()
{}


void FindSuccessDialog::SetSearchInfo(std::shared_ptr<SearchInfo> searchInfo)
{
    ui.label_name->setText(searchInfo->m_name);
    m_searchInfo = searchInfo;
}

void FindSuccessDialog::onButtonAddFriendClicked()
{
    //TODO: 添加好友界面弹出
}

