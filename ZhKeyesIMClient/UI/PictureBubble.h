#ifndef ZHKEYESIMCLIENT_UI_PICTUREBUBBLE_H
#define ZHKEYESIMCLIENT_UI_PICTUREBUBBLE_H

#include "BubbleFrame.h"
#include <QHBoxLayout>
#include <QPixmap>
class PictureBubble : public BubbleFrame
{
    Q_OBJECT
public:
    PictureBubble(const QPixmap& picture, ChatRole role, QWidget* parent = nullptr);
};

#endif // PICTUREBUBBLE_H
