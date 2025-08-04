# ==================== 服务端配置 ====================

# 定义服务端源文件
set(SERVER_SOURCES
    main.cpp          # 主程序入口
)

# ==================== 创建可执行文件 ====================
# 创建服务端可执行文件
add_executable(ZhkeyesIMServer ${SERVER_SOURCES})

# ==================== 包含目录设置 ====================
# 设置包含目录
target_include_directories(ZhkeyesIMServer PRIVATE
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