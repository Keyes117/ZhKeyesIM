#ifndef ZHKEYESIMSERVER_MODEL_FRIENDAPPLY_H_
#define ZHKEYESIMSERVER_MODEL_FRIENDAPPLY_H_

#include <cstdint>
#include <string>

struct FriendApply {
    FriendApply() : id(0), fromUid(0), toUid(0), status(0), applyTime(0) {}

    int32_t id;
    int32_t fromUid;
    int32_t toUid;
    int32_t status;     // 0=待处理, 1=已同意, 2=已拒绝
    int64_t applyTime;
};

struct FriendApplyInfo
{
    int32_t  fromUid;   // 对应 ApplyInfo::_uid
    std::string name;   // _name
    std::string desc;   // _desc
    std::string icon;   // _icon
    std::string nick;   // _nick
    uint32_t sex;       // _sex
    uint32_t status;    // _status
};

#endif