# ==================== 服务端配置 ====================



# 定义服务端源文件
set(SERVER_SOURCES
    IMServer.cpp
    IMServer.h
    IMSession.h
    IMSession.cpp
)

file(GLOB SERVER_IMPROTOCOL_SOURCES
    "${CMAKE_SOURCE_DIR}/IMProtocol/*.cpp"
    "${CMAKE_SOURCE_DIR}/IMProtocol/*.h"
)

# ==================== 创建可执行文件 ====================
# 创建服务端可执行文件
add_executable(ZhkeyesIMServer 
    main.cpp   
    ${SERVER_SOURCES}
    ${SERVER_IMPROTOCOL_SOURCES}
)

source_group(
  TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${SERVER_SOURCES} main.cpp
)
# 单独处理公共 IMProtocol 文件
source_group(
  TREE ${CMAKE_SOURCE_DIR}
  FILES ${SERVER_IMPROTOCOL_SOURCES}
)

set_target_properties(ZhKeyesIMClient PROPERTIES AUTOGEN_SOURCE_GROUP "Generated")

configure_target_output(ZhkeyesIMServer "ZhkeyesIMServer")
copy_mysql_dlls_to_target(ZhkeyesIMServer)
copy_fmt_dlls_to_target(ZhkeyesIMServer)

# ==================== 包含目录设置 ====================
# 设置包含目录
target_include_directories(ZhkeyesIMServer PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/net   # 网络库头文件路径
    ${CMAKE_SOURCE_DIR}/util  # 工具库头文件路径
    ${CMAKE_SOURCE_DIR}/Log
)

# ==================== 依赖库链接 ====================
# 链接动态库（会自动链接导入库）
target_link_libraries(ZhkeyesIMServer PRIVATE net util Log)

# ==================== 平台特定设置 ====================
if(WIN32)
    target_compile_definitions(ZhkeyesIMServer PRIVATE _WIN32)
    target_link_libraries(ZhkeyesIMServer PRIVATE ws2_32)
endif()

# 安装可执行文件
install(TARGETS ZhkeyesIMServer
    RUNTIME DESTINATION bin
    CONFIGURATIONS Debug Release
)