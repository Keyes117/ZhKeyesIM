#ifndef ZHKEYESIMSERVER_MODEL_FRIENDAPPLY_H_
#define ZHKEYESIMSERVER_MODEL_FRIENDAPPLY_H_

#include <cstdint>

struct FriendApply {
    FriendApply() : id(0), fromUid(0), toUid(0), status(0), applyTime(0) {}

    int32_t id;
    int32_t fromUid;
    int32_t toUid;
    int32_t status;     // 0=待处理, 1=已同意, 2=已拒绝
    int64_t applyTime;
};

#endif