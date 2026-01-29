# ==================== 服务端配置 ====================

set(SERVER_SOURCES
    main.cpp          # 主程序入口
    GateServer.cpp
    message.pb.cc
    message.grpc.pb.cc

    controller/BaseController.cpp
    controller/UserController.cpp
    controller/VerifyController.cpp

    repository/UserRepository.cpp
    repository/RedisRepository.cpp

    service/AuthService.cpp
    service/UserService.cpp
    service/VerifyService.cpp

)

set(SERVER_HEADERS
    const.h
    GateServer.h
    message.pb.h
    message.grpc.pb.h


    controller/BaseController.h
    controller/UserController.h
    controller/VerifyController.h

    model/User.h
    model/ServiceResult.h

    repository/UserRepository.h
    repository/RedisRepository.h

    service/AuthService.h
    service/UserService.h
    service/VerifyService.h
)

set(infrastructure_SOURCES
    ${CMAKE_SOURCE_DIR}/infrastructure/VerifyGrpcClient.cpp
    ${CMAKE_SOURCE_DIR}/infrastructure/StatusGrpcClient.cpp
    ${CMAKE_SOURCE_DIR}/infrastructure/MySqlConnPool.cpp
    ${CMAKE_SOURCE_DIR}/infrastructure/MySqlManager.cpp
    ${CMAKE_SOURCE_DIR}/infrastructure/RedisManager.cpp
    ${CMAKE_SOURCE_DIR}/infrastructure/RedisConnPool.cpp
)

set(infrastructure_HEADERS
    ${CMAKE_SOURCE_DIR}/infrastructure/VerifyGrpcClient.h
    ${CMAKE_SOURCE_DIR}/infrastructure/StatusGrpcClient.h
    ${CMAKE_SOURCE_DIR}/infrastructure/MySqlConnPool.h
    ${CMAKE_SOURCE_DIR}/infrastructure/MySqlManager.h
    ${CMAKE_SOURCE_DIR}/infrastructure/RedisManager.h
    ${CMAKE_SOURCE_DIR}/infrastructure/RedisConnPool.h
)


# ==================== 创建可执行文件 ====================
# 创建服务端可执行文件
add_executable(GateServer ${SERVER_SOURCES} ${SERVER_HEADERS}
         ${infrastructure_SOURCES} ${infrastructure_HEADERS})


source_group(
  TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${SERVER_SOURCES} ${SERVER_HEADERS}
)


source_group(
  TREE ${CMAKE_SOURCE_DIR}
  FILES ${infrastructure_SOURCES} ${infrastructure_HEADERS}
)

set_target_properties(GateServer  PROPERTIES AUTOGEN_SOURCE_GROUP "Generated")


configure_target_output(GateServer "GateServer")

# 设置 Visual Studio 调试工作目录为可执行文件所在目录
set_target_properties(GateServer PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:GateServer>"
)
# ==================== 包含目录设置 ====================
# 设置包含目录
target_include_directories(GateServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/net   # 网络库头文件路径
    ${CMAKE_SOURCE_DIR}/util  # 工具库头文件路径
    ${CMAKE_SOURCE_DIR}/Log
)

# ==================== 依赖库链接 ====================
# 链接动态库（会自动链接导入库）
target_link_libraries(GateServer PRIVATE 
        net
        Log
        util
        gRPC::grpc++
        protobuf::libprotobuf  
        hiredis::hiredis
        mysqlcppconn::mysqlcppconn

)

# ==================== 复制配置文件到输出目录 ====================
set(CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/config.json")

if(EXISTS "${CONFIG_FILE}")
    # 复制配置文件到输出目录（构建后）
    add_custom_command(TARGET GateServer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CONFIG_FILE}"
        "$<TARGET_FILE_DIR:GateServer>/config.json"
        COMMENT "Copying config.json to GateServer output directory"
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

# ==================== 平台特定设置 ====================

if(WIN32)
    copy_mysql_dlls_to_target(GateServer)
    copy_fmt_dlls_to_target(GateServer)
    copy_project_dlls_to_target(GateServer)
    target_compile_definitions(GateServer PRIVATE _WIN32)
     # 1. 自动查找所有 Abseil 库 (避免手动列出导致遗漏)
     file(GLOB ABSL_LIBS "${GRPC_DIR}/lib/absl_*.lib")

     # 2. 链接所有 Windows 依赖和第三方库静态库
     target_link_libraries(GateServer PRIVATE 
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
install(TARGETS GateServer
    RUNTIME DESTINATION bin
    CONFIGURATIONS Debug Release
)
