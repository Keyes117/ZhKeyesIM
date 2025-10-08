set(CLIENT_NETWORK_SOURCE
    NetWork/IMClient.cpp
    NetWork/IMClient.h
)

set(CLIENT_UI_SOURCE
    UI/LoginDlg.cpp
    UI/LoginDlg.h
    UI/LoginDlg.ui
    UI/RegisterDlg.cpp
    UI/RegisterDlg.h
    UI/RegisterDlg.ui
)

set(CLIENT_TASK_SOURCE
    Task/Task.cpp
    Task/Task.h
    Task/TaskHandler.cpp
    Task/TaskHandler.h
)

# Qt配置
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 创建客户端可执行文件
add_executable(ZhKeyesIMClient 
    main.cpp   
    ${CLIENT_NETWORK_SOURCE} 
    ${CLIENT_UI_SOURCE}
    ${CLIENT_TASK_SOURCE}
)

source_group(
  TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${CLIENT_NETWORK_SOURCE} ${CLIENT_UI_SOURCE} ${CLIENT_TASK_SOURCE} main.cpp
)

set_target_properties(ZhKeyesIMClient PROPERTIES AUTOGEN_SOURCE_GROUP "Generated")

# 包含目录
target_include_directories(ZhKeyesIMClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/UI
    ${CMAKE_CURRENT_SOURCE_DIR}/Task
    ${CMAKE_CURRENT_SOURCE_DIR}/NetWork
    ${CMAKE_SOURCE_DIR}/net
    ${CMAKE_SOURCE_DIR}/util
    ${CMAKE_SOURCE_DIR}/Log
)

# 链接库
target_link_libraries(ZhKeyesIMClient PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    Log
    net
    util
)

# 平台特定设置
if(WIN32)
    target_compile_definitions(ZhKeyesIMClient PRIVATE _WIN32)
    set_target_properties(ZhKeyesIMClient
        PROPERTIES
            WIN32_EXECUTABLE TRUE
    )
endif()

# 安装规则
install(TARGETS ZhKeyesIMClient
    RUNTIME DESTINATION bin
)