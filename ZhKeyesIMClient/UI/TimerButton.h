#ifndef ZHKEYESIMCLIENT_UI_TIMERBUTTON_H_
#define ZHKEYESIMCLIENT_UI_TIMERBUTTON_H_


#include <QPushButton>
#include <QTimer>

class TimerButton  : public QPushButton
{
    Q_OBJECT

public:
    TimerButton(QWidget *parent);
    ~TimerButton();

    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QTimer* m_timer;
    int     m_counter;
};

#endif //!UI_TIMERBUTTON_H_