#include "ChatDialog.h"

#include "ClickedButton.h"

#include <QAction>
#include <QIcon>

ChatDialog::ChatDialog(std::shared_ptr<IMClient> spClient, QWidget* parent) :
    m_spClient(spClient), QDialog(parent)
{
    ui.setupUi(this);

    ui.button_add->SetState("normal", "hover", "press");

    m_searchAction = new QAction(ui.lineEdit_search);
    m_searchAction->setIcon(QIcon(":/res/res/search.png"));
    ui.lineEdit_search->addAction(m_searchAction, QLineEdit::LeadingPosition);
    ui.lineEdit_search->setPlaceholderText(QString::fromLocal8Bit("ËÑË÷"));

    m_clearAction = new QAction(ui.lineEdit_search);
    m_clearAction->setIcon(QIcon(":/res/res/close_transparent.png"));

    ui.lineEdit_search->addAction(m_clearAction, QLineEdit::TrailingPosition);

    connect(ui.lineEdit_search, &QLineEdit::textChanged, this, &ChatDialog::onLineEditSearchChanged);
    connect(m_clearAction, &QAction::triggered, this, &ChatDialog::onClearActionTriggered);

    ui.lineEdit_search->setMaxLength(15);
}

ChatDialog::~ChatDialog()
{}

void ChatDialog::onLineEditSearchChanged(const QString& text)
{
    if (!text.isEmpty())
    {
        m_clearAction->setIcon(QIcon(":/res/res/close_search.png"));
    }
    else
    {
        m_clearAction->setIcon(QIcon(":/res/res/close_transparent.png"));
    }

}
void ChatDialog::onClearActionTriggered()
{
    ui.lineEdit_search->clear();
    m_clearAction->setIcon(QIcon(":/res/res/close_transparent.png"));
    ui.lineEdit_search->clearFocus();

}