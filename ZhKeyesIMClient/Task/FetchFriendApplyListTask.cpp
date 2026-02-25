#include "FetchFriendApplyListTask.h"

#include "IMProtocol/BinaryReader.h"
#include "Base/UserSession.h"

FetchFriendApplyListTask::FetchFriendApplyListTask(Task::ConstructorKey key, 
    Task::TaskId id, 
    std::shared_ptr<IMClient> client,
    uint32_t uid):
    Task(key,id,Task::TaskType::TASK_TYPE_FETCHFRIENDAPPLYLIST),
    m_spClient(client),
    m_uid(uid)
{
}

void FetchFriendApplyListTask::doTask()
{
    auto self = std::static_pointer_cast<FetchFriendApplyListTask>(shared_from_this());
    std::weak_ptr<FetchFriendApplyListTask> weakSelf = self;

    // 调用 IMClient 的方法发送请求
    m_spClient->fetchFriendApplyList(
        m_uid,
        [weakSelf](std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg,
            std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender) {
                if (auto selfLocked = weakSelf.lock()) {
                    selfLocked->onFetchFriendApplyListResponse(msg, sender);
                }
        },
        [weakSelf](const std::string& err) {
            if (auto selfLocked = weakSelf.lock()) {
                selfLocked->onTaskError(err);
            }
        }
    );
}

void FetchFriendApplyListTask::onFetchFriendApplyListResponse
    (std::shared_ptr<ZhKeyesIM::Protocol::IMMessage> msg, 
    std::shared_ptr<ZhKeyesIM::Protocol::IMMessageSender> sender)
{
    if (!msg || !msg->hasBody()) {
        LOG_WARN("FetchFriendApplyListTask: 响应消息体为空");
        onTaskError("响应消息体为空");
        return;
    }

    ZhKeyesIM::Protocol::BinaryReader reader(msg->getBody());

    uint8_t success = 0;
    if (!reader.readUInt8(success)) {
        onTaskError("响应解析失败");
        return;
    }

    if (success == 0) {
        std::string err;
        if (!reader.readString(err))
            err = "获取好友申请列表失败";
        onTaskError(err);
        return;
    }

    // 读取申请列表数量
    uint32_t count = 0;
    if (!reader.readUInt32(count)) {
        onTaskError("响应解析失败：无法读取列表数量");
        return;
    }

    // 清空现有列表（可选，取决于业务需求）
    // UserSession::getInstance().ClearApplyList();

    // 读取每个申请
    for (uint32_t i = 0; i < count; ++i) {
        uint32_t fromUid = 0;
        std::string name, nick, desc, icon;
        uint32_t sex = 0;
        uint32_t status = 0;  // 0=待处理, 1=已同意, 2=已拒绝

        if (!reader.readUInt32(fromUid) ||
            !reader.readString(name) ||
            !reader.readString(nick) ||
            !reader.readString(desc) ||
            !reader.readUInt32(sex) ||
            !reader.readString(icon) ||
            !reader.readUInt32(status)) {
            LOG_WARN("FetchFriendApplyListTask: 解析第 %u 个申请失败", i);
            continue;  // 跳过这个，继续解析下一个
        }

        // 创建 AddFriendApply 对象
        auto apply = std::make_shared<AddFriendApply>(
            fromUid,
            QString::fromStdString(name),
            QString::fromStdString(desc),
            QString::fromStdString(icon),
            QString::fromStdString(nick),
            sex
        );

        // 创建 ApplyInfo 并设置状态
        auto applyInfo = std::make_shared<ApplyInfo>(apply);
        applyInfo->m_status = status;

        // 保存到 UserSession
        UserSession::getInstance().addApply(std::move(applyInfo));
    }

    LOG_INFO("FetchFriendApplyListTask: 成功拉取 %u 条好友申请", count);
    onTaskSuccess();
}
