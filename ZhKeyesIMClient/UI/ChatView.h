#ifndef ZHKEYESIMCLIENT_UI_CHATVIEW_H_
#define ZHKEYESIMCLIENT_UI_CHATVIEW_H_

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>

class ChatView : public QWidget
{
    Q_OBJECT
public:
    ChatView(QWidget* parent = nullptr);
    void appendChatItem(QWidget* item);
    void prependChatItem(QWidget* item);
    void insertChatItem(QWidget* before, QWidget* item);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onVScrollBarMoved(int min, int max);

private:
    void initStyleSheet();

private:
    QVBoxLayout* m_pVLayout;
    QScrollArea* m_pScrollArea;
    bool m_isAppended = false;
};

#endif