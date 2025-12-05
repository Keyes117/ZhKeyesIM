#ifndef UI_GLOBAL_H_
#define UI_GLOBAL_H_

#include <QWidget>
#include <functional>
#include <QStyle>
#include <QWidget>


/**
 * @brief repolish ?qss
 */

extern std::function<void(QWidget*)> repolish;

#endif