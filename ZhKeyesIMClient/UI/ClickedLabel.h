#ifndef UI_CLICKEDLABEL_H_
#define UI_CLICKEDLABEL_H_

#include <QLabel>
#include <QMouseEvent>
#include <QEnterEvent>

#include "global.h"


class ClickedLabel : public QLabel
{
    Q_OBJECT
public:
    ClickedLabel(QWidget* parent);

    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

    void setState(const QString& normal = "", const QString& hover = "", const QString& press = "",
        const QString& select = "", const QString& select_hover = "", const QString& select_press = ""
    );

    ClickState getCurState() { return m_curState; }
signals:
    void clicked(void);

private:
    QString m_normal;
    QString m_normal_hover;
    QString m_normal_press;

    QString m_selected;
    QString m_selected_hover;
    QString m_selected_press;

    ClickState m_curState;
};


#endif //!UI_CLICKEDLABEL_H_