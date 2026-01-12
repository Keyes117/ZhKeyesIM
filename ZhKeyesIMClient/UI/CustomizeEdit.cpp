#include "CustomizeEdit.h"

CustomizeEdit::CustomizeEdit(QWidget* parent):
    QLineEdit(parent)
{
}

void CustomizeEdit::setMaxLength(int maxLen)
{
    m_maxLength = maxLen;
}

void CustomizeEdit::focusOutEvent(QFocusEvent* event)
{
    QLineEdit::focusOutEvent(event);
    //发送失去焦点得信号
    emit lineEditFocusOut();
}

void CustomizeEdit::limitTextLength(QString text)
{
    if (m_maxLength <= 0) {
        return;
    }
    QByteArray byteArray = text.toUtf8();
    if (byteArray.size() > m_maxLength) {
        byteArray = byteArray.left(m_maxLength);
        this->setText(QString::fromUtf8(byteArray));
    }
}
