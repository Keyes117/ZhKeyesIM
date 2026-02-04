# ==================== 服务端配置 ====================

# 定义服务端源文件
set(SERVER_SOURCES
    IMServer.cpp
    IMServer.h
    IMSession.h
    IMSession.cpp
)

file(GLOB SERVER_PROTOBUF_SOURCES
    "${CMAKE_SOURCE_DIR}/protobuf/*.pb.cc"
    "${CMAKE_SOURCE_DIR}/protobuf/*.pb.h"
    "${CMAKE_SOURCE_DIR}/protobuf/*.proto"
)

file(GLOB SERVER_INFRASTRUCTURE_SOURCES
    "${CMAKE_SOURCE_DIR}/infrastructure/ChatGrpcClient.cpp"
    "${CMAKE_SOURCE_DIR}/infrastructure/ChatGrpcClient.h"
    "${CMAKE_SOURCE_DIR}/infrastructure/ChatGrpcServer.cpp"
    "${CMAKE_SOURCE_DIR}/infrastructure/ChatGrpcServer.h"

    "${CMAKE_SOURCE_DIR}/infrastructure/MySqlConnPool.cpp"
    "${CMAKE_SOURCE_DIR}/infrastructure/MySqlConnPool.h"
    "${CMAKE_SOURCE_DIR}/infrastructure/MySqlManager.cpp"
    "${CMAKE_SOURCE_DIR}/infrastructure/MySqlManager.h"

    "${CMAKE_SOURCE_DIR}/infrastructure/RedisConnPool.cpp"
    "${CMAKE_SOURCE_DIR}/infrastructure/RedisConnPool.h"
    "${CMAKE_SOURCE_DIR}/infrastructure/RedisManager.cpp"
    "${CMAKE_SOURCE_DIR}/infrastructure/RedisManager.h"
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
    ${SERVER_PROTOBUF_SOURCES}
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

source_group(
  TREE ${CMAKE_SOURCE_DIR}
  FILES ${SERVER_IMPROTOCOL_SOURCES} ${SERVER_INFRASTRUCTURE_SOURCES} ${SERVER_PROTOBUF_SOURCES}
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
target_link_libraries(ZhkeyesIMServer PRIVATE 
        net 
        util 
        Log
        gRPC::grpc++
        protobuf::libprotobuf      
        hiredis::hiredis
        mysqlcppconn::mysqlcppconn
    )

# ==================== 平台特定设置 ====================
if(WIN32)
    copy_project_dlls_to_target(ZhkeyesIMServer)
    target_compile_definitions(ZhkeyesIMServer PRIVATE _WIN32)
     # 2. 链接所有 Windows 依赖和第三方库静态库
     target_link_libraries(ZhkeyesIMServer PRIVATE 
         ws2_32
         Crypt32  # OpenSSL 依赖的系统库
         User32   # OpenSSL 可能依赖
         
        # gRPC 核心库
        ${GRPC_DIR}/lib/grpc.lib
        ${GRPC_DIR}/lib/gpr.lib
        

         ${GRPC_DIR}/lib/libssl.lib
         ${GRPC_DIR}/lib/libcrypto.lib

         # Protobuf 依赖的 utf8 验证库 (解决 utf8_range 错误)
         ${GRPC_DIR}/lib/utf8_validity.lib
         ${GRPC_DIR}/lib/utf8_range.lib
         
         # gRPC 核心依赖库
         ${GRPC_DIR}/lib/address_sorting.lib
         ${GRPC_DIR}/lib/cares.lib
         ${GRPC_DIR}/lib/zlibstaticd.lib
         ${GRPC_DIR}/lib/re2.lib
         
         # upb (micro protobuf) 库，gRPC 可能用到
         ${GRPC_DIR}/lib/upb_base_lib.lib
         ${GRPC_DIR}/lib/upb_mem_lib.lib
         ${GRPC_DIR}/lib/upb_message_lib.lib
 
         # 链接上面找到的所有 Abseil 库
         ${ABSL_LIBS}
    )

endif()

# 安装可执行文件
install(TARGETS ZhkeyesIMServer
    RUNTIME DESTINATION bin
    CONFIGURATIONS Debug Release
)
