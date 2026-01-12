#ifndef UI_CUSTOMIZEEDIT_H_
#define UI_CUSTOMIZEEDIT_H_

#include <QLineEdit>
#include <QFocusEvent>

class CustomizeEdit : public QLineEdit
{
    Q_OBJECT
public:
    CustomizeEdit(QWidget* parent = nullptr);
    void setMaxLength(int maxLen);

protected:
    virtual void focusOutEvent(QFocusEvent* event);

signals:
    void lineEditFocusOut();

private:
    void limitTextLength(QString text);

    int m_maxLength;
};

#endif