set(NET_SOURCES
    Http/HttpMessage.cpp
    Http/HttpSession.cpp
    Http/HttpServer.cpp
    Http/HttpClient.cpp
    Http/HttpResponse.cpp
    Http/HttpRequest.cpp
    Http/HttpProtocol.cpp
    Http/HttpParser.cpp
    Http/Router.cpp
    Acceptor.cpp
    Buffer.cpp
    Epoll.cpp
    EventLoop.cpp
    Poll.cpp
    Select.cpp
    TCPClient.cpp
    TCPConnection.cpp
    TCPConnector.cpp
    TCPServer.cpp
    ThreadPool.cpp
    SocketUtil.cpp
    TimerService.cpp
    Timer.cpp
    WakeUpEventDispatcher.cpp
)

set(NET_HEADERS
    Http/HttpMessage.h
    Http/HttpSession.h
    Http/HttpServer.h
    Http/HttpClient.h
    Http/HttpResponse.h
    Http/HttpRequest.h
    Http/HttpProtocol.h
    Http/HttpParser.h
    Http/Router.h
    Acceptor.h
    Buffer.h
    common.h
    Epoll.h
    EventDispatcher.h
    EventLoop.h
    IOMultiplex.h
    net_export.h
    Poll.h
    Select.h
    TCPClient.h
    TCPConnection.h
    TCPConnector.h
    TCPServer.h
    ThreadPool.h    
    TimerService.h
    Timer.h
    WakeUpEventDispatcher.h
)


add_library(net SHARED ${NET_SOURCES} ${NET_HEADERS})

source_group(
    TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${NET_SOURCES} ${NET_HEADERS}
)

set_target_properties(net  PROPERTIES
    OUTPUT_NAME "net"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_include_directories(net PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/Log>
    $<INSTALL_INTERFACE:include>

)

target_link_libraries(net PUBLIC Log util fmt::fmt)

target_compile_definitions(net PRIVATE NET_EXPORTS)

if(WIN32)
    target_compile_definitions(net PRIVATE 
        _WIN32
        _CRT_NO_POSIX_ERROR_CODES
    )
    
    # 链接Windows Socket库
    target_link_libraries(net PRIVATE ws2_32)
    
    # Windows下设置动态库属性
    set_target_properties(net PROPERTIES

        
        # 设置导入库的输出名称
        IMPORT_PREFIX "lib"
        
        # 设置动态库的输出名称
        PREFIX ""
    )
elseif(UNIX AND NOT APPLE)
    # 链接pthread库（POSIX线程库）
    target_link_libraries(net PRIVATE pthread)
    
    copy_fmt_dlls_to_target(net)

    # Linux下设置符号可见性
    set_target_properties(net PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
endif()

# 将目标添加到导出列表
install(TARGETS net
    EXPORT ZhKeyesIMTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)



install(FILES ${NET_HEADERS} DESTINATION include/net)