#ifndef UI_GLOBAL_H_
#define UI_GLOBAL_H_

#include <QWidget>
#include <functional>
#include <QStyle>
#include <QWidget>


/**
 * @brief repolish ùùùù?ùùqss
 */

extern std::function<void(QWidget*)> repolish;

enum class TipErr {
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};

enum class ClickState
{
    Normal = 0,
    Selected = 1
};

#endif