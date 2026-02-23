#ifndef ZHKEYESIMCLIENT_TASK_TASKBUILDER_H_
#define ZHKEYESIMCLIENT_TASK_TASKBUILDER_H_

#include <atomic>
#include <cstdint>
#include <memory>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

#include "Task/Task.h"
#include "Task/HttpResponseTask.h"
#include "NetWork/IMClient.h"


/**
 * @brief 任务统一构建类
 */
class TaskFactory
{
public:
    ~TaskFactory() = default;

    static TaskFactory& getInstance();

    bool init(std::shared_ptr<IMClient> client);

    // ==================== 任务构建方法 ====================
    
    template<class TaskT, typename... Args>
    std::shared_ptr<TaskT> buildTask(Args&&... args);

private:
    /*
    * SFINAE 原理
        编译器在匹配模板时，如果替换失败，会静默忽略该候选，继续尝试其他候选。
    */

    template<class TaskT, typename... Args>
    static constexpr bool needsClient() {
        return std::is_constructible_v<
            TaskT,
            Task::ConstructorKey,
            Task::TaskId,
            std::shared_ptr<IMClient>,
            Args...
        >;
    }

    // 需要 client 的版本
    template<class TaskT, typename... Args>
    std::shared_ptr<TaskT> buildTaskImpl(std::true_type, Args&&... args) {
        Task::TaskId taskId = generateTaskId();
        return std::make_shared<TaskT>(
            Task::ConstructorKey{},
            taskId,
            m_client,  
            std::forward<Args>(args)...
        );
    }

    // 不需要 client 的版本
    template<class TaskT, typename... Args>
    std::shared_ptr<TaskT> buildTaskImpl(std::false_type, Args&&... args) {
        Task::TaskId taskId = generateTaskId();
        return std::make_shared<TaskT>(
            Task::ConstructorKey{},
            taskId,
            std::forward<Args>(args)...  // 不注入 client
        );
    }

private:
    TaskFactory() = default;
    Task::TaskId generateTaskId();

    std::shared_ptr<IMClient> m_client;
};

#endif

template<class TaskT, typename ...Args>
inline std::shared_ptr<TaskT> TaskFactory::buildTask(Args && ...args)
{
    static_assert(std::is_base_of<Task, TaskT>::value,
        "TaskT must derive from Task");

    // 使用 std::integral_constant 作为标签，根据 needsClient 的结果选择重载
    using needs_client = std::integral_constant<bool, needsClient<TaskT, Args...>()>;

    return buildTaskImpl<TaskT>(needs_client{}, std::forward<Args>(args)...);
}
