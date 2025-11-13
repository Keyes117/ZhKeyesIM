# ==================== 服务端配置 ====================

set(SERVER_SOURCES
    main.cpp          # 主程序入口
    GateServer.cpp
    VerifyGrpcClient.cpp
    message.pb.cc
    message.grpc.pb.cc
)

set(SERVER_HEADERS
    const.h
    GateServer.h
    VerifyGrpcClient.h
    message.pb.h
    message.grpc.pb.h
)

# ==================== 创建可执行文件 ====================
# 创建服务端可执行文件
add_executable(GateServer ${SERVER_SOURCES} ${SERVER_HEADERS})

# ==================== 包含目录设置 ====================
# 设置包含目录
target_include_directories(GateServer PRIVATE
    ${CMAKE_SOURCE_DIR}/net   # 网络库头文件路径
    ${CMAKE_SOURCE_DIR}/util  # 工具库头文件路径
    ${CMAKE_SOURCE_DIR}/Log
    ${JSON_DIR}/include
    ${PROTOBUF_DIR}/include
    ${GRPC_DIR}/include
    
)

# ==================== 依赖库链接 ====================
# 链接动态库（会自动链接导入库）
target_link_libraries(GateServer PRIVATE 
        net
        util
        Log 
        nlohmann::json
        protobuf::libprotobuf
        gRPC::grpc++
)

# ==================== 平台特定设置 ====================
if(WIN32)
    target_compile_definitions(GateServer PRIVATE _WIN32)
    target_link_libraries(GateServer PRIVATE ws2_32)
endif()

# 安装可执行文件
install(TARGETS GateServer
    RUNTIME DESTINATION bin
    CONFIGURATIONS Debug Release
)
