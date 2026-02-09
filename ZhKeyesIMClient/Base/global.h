#ifndef UI_GLOBAL_H_
#define UI_GLOBAL_H_

#include <QWidget>
#include <QStyle>
#include <QString>

#include <functional>
#include <vector>

//申请好友标签输入框最低长度
const int MIN_APPLY_LABEL_ED_LEN = 40;

const QString add_prefix = /*QString::fromLocal8Bit(*/"添加标签 ";/*);*/

const int CHAT_COUNT_PER_PAGE = 13;

const int  tip_offset = 5;

static std::vector<QString>  strs = { "hello world !",
                             "nice to meet u",
                             "New year，new life",
                            "You have to love yourself",
                            "My love is written in the wind ever since the whole world is you" };
static std::vector<QString> heads = {
    ":/res/res/head_1.jpg",
    ":/res/res/head_2.jpg",
    ":/res/res/head_3.jpg",
    ":/res/res/head_4.jpg",
    ":/res/res/head_5.jpg"
};
static std::vector<QString> names = {
    "llfc",
    "zack",
    "golang",
    "cpp",
    "java",
    "nodejs",
    "python",
    "rust"
};

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

//QListWidgetItem的几种类型
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

#endif