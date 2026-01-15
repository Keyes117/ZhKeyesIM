#ifndef UI_GLOBAL_H_
#define UI_GLOBAL_H_

#include <QWidget>
#include <functional>
#include <QStyle>
#include <QWidget>


/**
 * 登录数据结构
 */
struct User {
    uint64_t uid = 0;
    std::string username;
    std::string email;
    std::string token;

    std::string chatServerHost;
    uint16_t chatSevrerPort;
};

/**
 * @brief repolish 潩潩?潩qss
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

enum class ClickLbState
{
    Normal = 0,
    Selected = 1
};


enum ChatUIMode {
    SearchMode, //搜索模式
    ChatMode, //聊天模式
    ContactMode, //联系模式
    SettingsMode, //设置模式
};

//自定义QListWidgetItem的几种类型
enum ListItemType {
    CHAT_USER_ITEM, //聊天用户
    CONTACT_USER_ITEM, //联系人用户
    SEARCH_USER_ITEM, //搜索到的用户
    ADD_USER_TIP_ITEM, //提示添加用户
    INVALID_ITEM,  //不可点击条目
    GROUP_TIP_ITEM, //分组提示条目
    LINE_ITEM,  //分割线
    APPLY_FRIEND_ITEM, //好友申请
};

enum class ChatRole
{
    Self,
    Other
};

struct MsgInfo {
    QString msgFlag;//"text,image,file"
    QString content;//表示文件和图像的url,文本信息
    QPixmap pixmap;//文件和图片的缩略图
};

class SearchInfo {
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex) :
        m_uid(uid), m_name(name), m_nick(nick), m_desc(desc), m_sex(sex)
    {

    }

    int     m_uid;
    QString m_name;
    QString m_nick;
    QString m_desc;
    int     m_sex;
};

#endif