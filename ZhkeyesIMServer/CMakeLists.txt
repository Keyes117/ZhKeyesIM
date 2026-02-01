# ==================== 服务端配置 ====================



# 定义服务端源文件
set(SERVER_SOURCES
    IMServer.cpp
    IMServer.h
    IMSession.h
    IMSession.cpp
)

file(GLOB SERVER_INFRASTRUCTURE_SOURCES
    "${CMAKE_SOURCE_DIR}/infrastructure/ChatGrpcClient.cpp"
    "${CMAKE_SOURCE_DIR}/infrastructure/ChatGrpcClient.h"
    "${CMAKE_SOURCE_DIR}/infrastructure/ChatGrpcServer.cpp"
    "${CMAKE_SOURCE_DIR}/infrastructure/ChatGrpcServer.h"
)

file(GLOB SERVER_IMPROTOCOL_SOURCES
    "${CMAKE_SOURCE_DIR}/IMProtocol/*.cpp"
    "${CMAKE_SOURCE_DIR}/IMProtocol/*.h"
)

file(GLOB SERVER_CONTROLLER_SOURCES
    "Controller/*.cpp"
    "Controller/*.h"
)
file(GLOB SERVER_SERVICE_SOURCES
    "Service/*.cpp"
    "Service/*.h"
)
file(GLOB SERVER_REPOSITORY_SOURCES
    "Repository/*.cpp"
    "Repository/*.h"
)

file(GLOB SERVER_MODEL_SOURCES
    "Model/*.cpp"
    "Model/*.h"
)

# ==================== 创建可执行文件 ====================
# 创建服务端可执行文件
add_executable(ZhkeyesIMServer 
    main.cpp   
    ${SERVER_SOURCES}
    ${SERVER_INFRASTRUCTURE_SOURCES}
    ${SERVER_IMPROTOCOL_SOURCES}
    ${SERVER_CONTROLLER_SOURCES}
    ${SERVER_SERVICE_SOURCES}
    ${SERVER_REPOSITORY_SOURCES}
    ${SERVER_MODEL_SOURCES}
)

# ==================== 配置文件 ====================
set(CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/config.json")

if(EXISTS "${CONFIG_FILE}")
    # 复制配置文件到输出目录（构建后）
    add_custom_command(TARGET ZhkeyesIMServer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CONFIG_FILE}"
        "$<TARGET_FILE_DIR:ZhkeyesIMServer>/config.json"
        COMMENT "Copying config.json to ZhkeyesIMServer output directory"
    )
    
    # 安装配置文件
    install(FILES "${CONFIG_FILE}"
        DESTINATION bin
        OPTIONAL
    )
else()
    message(WARNING "Config file not found: ${CONFIG_FILE}")
    message(STATUS "You may need to create config.json manually in the output directory")
endif()

# 设置 Visual Studio 调试工作目录为可执行文件所在目录
set_target_properties(ZhkeyesIMServer PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:ZhkeyesIMServer>"
)


source_group(
  TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${SERVER_SOURCES} ${SERVER_CONTROLLER_SOURCES} ${SERVER_SERVICE_SOURCES} ${SERVER_REPOSITORY_SOURCES} ${SERVER_MODEL_SOURCES} main.cpp
)
# 单独处理公共 IMProtocol 文件
source_group(
  TREE ${CMAKE_SOURCE_DIR}
  FILES ${SERVER_IMPROTOCOL_SOURCES} ${SERVER_INFRASTRUCTURE_SOURCES}
)

set_target_properties(ZhkeyesIMServer PROPERTIES AUTOGEN_SOURCE_GROUP "Generated")

configure_target_output(ZhkeyesIMServer "ZhkeyesIMServer")
copy_mysql_dlls_to_target(ZhkeyesIMServer)
copy_fmt_dlls_to_target(ZhkeyesIMServer)

# ==================== 包含目录设置 ====================
# 设置包含目录
target_include_directories(ZhkeyesIMServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
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
    copy_project_dlls_to_target(ZhkeyesIMServer)
    target_compile_definitions(ZhkeyesIMServer PRIVATE _WIN32)
    target_link_libraries(ZhkeyesIMServer PRIVATE ws2_32)
endif()

# 安装可执行文件
install(TARGETS ZhkeyesIMServer
    RUNTIME DESTINATION bin
    CONFIGURATIONS Debug Release
)
