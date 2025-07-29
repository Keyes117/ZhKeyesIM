# ==================== 客户端配置 ====================

# 定义客户端源文件
set(CLIENT_SOURCES
    main.cpp          # 主程序入口
    IMClient.cpp      # IM客户端实现
)

# 定义客户端头文件
set(CLIENT_HEADERS
    IMClient.h        # IM客户端头文件
)

# ==================== 创建可执行文件 ====================
# 创建客户端可执行文件
add_executable(ZhKeyesIMClient ${CLIENT_SOURCES} ${CLIENT_HEADERS})

# ==================== 包含目录设置 ====================
# 设置包含目录
target_include_directories(ZhKeyesIMClient PRIVATE
    ${CMAKE_SOURCE_DIR}/net   # 网络库头文件路径
    ${CMAKE_SOURCE_DIR}/util  # 工具库头文件路径
)

# ==================== 依赖库链接 ====================
# 链接动态库（会自动链接导入库）
target_link_libraries(ZhKeyesIMClient PRIVATE net util)

# ==================== 平台特定设置 ====================
if(WIN32)
    target_compile_definitions(ZhKeyesIMClient PRIVATE _WIN32)
    target_link_libraries(ZhKeyesIMClient PRIVATE ws2_32)
endif()

# 安装可执行文件
install(TARGETS ZhKeyesIMClient
    RUNTIME DESTINATION bin
    CONFIGURATIONS Debug Release
)