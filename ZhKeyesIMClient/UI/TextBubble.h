#ifndef ZHKEYESIMCLIENT_UI_TEXTBUBBLE_H_
#define ZHKEYESIMCLIENT_UI_TEXTBUBBLE_H_

#include "UI/BubbleFrame.h"
#include <QTextEdit>

#include <QHBoxLayout>

class TextBubble : public BubbleFrame
{
    Q_OBJECT
public:
    TextBubble(ChatRole role, const QString& text, QWidget* parent = nullptr);
protected:
    bool eventFilter(QObject* o, QEvent* e);
private:
    void adjustTextHeight();
    void setPlainText(const QString& text);
    void initStyleSheet();
private:
    QTextEdit* m_pTextEdit;
};

#endif
