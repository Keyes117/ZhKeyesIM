#ifndef ZHKEYESIMCLIENT_UI_STATEWIDGET_H_
#define ZHKEYESIMCLIENT_UI_STATEWIDGET_H_

#include <QLabel>
#include "Base/global.h"
#include <qcoreevent.h>

class QLabel;

class StateWidget : public QLabel
{
    Q_OBJECT
public:
    explicit StateWidget(QWidget* parent = nullptr);

    void setState(const QString& normal = "", const QString& hover = "", const QString& press = "",
        const QString& select = "", const QString& select_hover = "", const QString& select_press = "");

    ClickLbState getCurState();
    void clearState();

    void setSelected(bool bSelected);
    void addRedPoint();
    void showRedPoint(bool show = true);



protected:
    virtual void paintEvent(QPaintEvent* event) override;


    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

signals:
    void clicked(void);

private:
    QString m_normal;
    QString m_normal_hover;
    QString m_normal_press;

    QString m_selected;
    QString m_selected_hover;
    QString m_selected_press;

    ClickLbState m_curState = ClickLbState::Normal;
    QLabel* m_red_point;
};


#endif