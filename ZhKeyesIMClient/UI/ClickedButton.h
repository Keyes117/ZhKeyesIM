#ifndef UI_CLICKEDBUTTON_H_
#define UI_CLICKEDBUTTON_H_

#include <QWidget>
#include <QPushButton>
#include <QEvent>

class ClickedButton : public QPushButton
{
    Q_OBJECT
public:
    ClickedButton(QWidget* parent = nullptr);
    ~ClickedButton();

    void SetState(QString normal, QString hover,QString press);

protected:
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;

    virtual void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QString m_normal;
    QString m_hover;
    QString m_press;
};



#endif