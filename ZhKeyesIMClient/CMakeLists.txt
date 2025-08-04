set(CLIENT_SOURCES
    main.cpp
    IMClient.cpp
    LoginDlg.cpp
)

set(CLIENT_HEADERS
    IMClient.h
    LoginDlg.h
)

set(CLIENT_UI_FILES
    LoginDlg.ui
)

# Qt配置
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
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
    ${CLIENT_SOURCES} 
    ${CLIENT_HEADERS}
    ${CLIENT_UI_FILES}
)

# 包含目录
target_include_directories(ZhKeyesIMClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
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