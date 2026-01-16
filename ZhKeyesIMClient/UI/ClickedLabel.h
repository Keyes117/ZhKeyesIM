#ifndef ZHKEYESIMCLIENT_UI_CLICKEDLABEL_H_
#define ZHKEYESIMCLIENT_UI_CLICKEDLABEL_H_

#include <QLabel>
#include <QMouseEvent>
#include <QEnterEvent>

#include "global.h"


class ClickedLabel : public QLabel
{
    Q_OBJECT
public:
    ClickedLabel(QWidget* parent);

    void setState(const QString& normal = "", const QString& hover = "", const QString& press = "",
        const QString& select = "", const QString& select_hover = "", const QString& select_press = ""
    );

    ClickLbState getCurState() { return m_curState; }
    void setCurState(ClickLbState state)
    {
        m_curState = state;
    }
    void resetNormalState()
    {
        m_curState = ClickLbState::Normal;
        setProperty("state", m_normal);
        repolish(this);
    }

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;


signals:
    void clicked(QString, ClickLbState);

private:
    QString m_normal;
    QString m_normal_hover;
    QString m_normal_press;

    QString m_selected;
    QString m_selected_hover;
    QString m_selected_press;

    ClickLbState m_curState;
};


#endif //!UI_CLICKEDLABEL_H_