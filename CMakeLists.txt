cmake_minimum_required(VERSION 3.16)
project(ZhKeyesIM VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 设置可执行文件的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(WIN32)
    # Windows: DLL 和 .exe 都放在 bin 目录
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    # Windows: 导入库（.lib）放在 lib 目录
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
elseif(UNIX AND NOT APPLE)
    # Linux: 共享库（.so）放在 lib 目录
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    # Linux: 静态库（.a）也放在 lib 目录
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

set(CMAKE_BUILD_RPATH "${CMAKE_BINARY_DIR}/lib")
# 启用测试支持
enable_testing()

# 设置第三方库路径
set(THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")
set(GTEST_ROOT "${THIRD_PARTY_DIR}/googletest")
set(FMT_DIR "${THIRD_PARTY_DIR}/fmt")
set(GRPC_DIR "${THIRD_PARTY_DIR}/grpc")
set(PROTOBUF_DIR "${THIRD_PARTY_DIR}/protobuf")
set(JSON_DIR "${THIRD_PARTY_DIR}/json")
set(REDIS_DIR "${THIRD_PARTY_DIR}/redis")

# 编译器特定设置 - 修复运行时库不匹配问题
if(MSVC)
    add_compile_options(/utf-8)

    # 统一设置运行时库
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    set_property(GLOBAL PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

    # 设置编译选项
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    
    # 为所有目标设置统一的运行时库
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MD")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MD")
    

        
    # 设置调试级别
    add_compile_definitions(
        $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=2>
        $<$<CONFIG:Release>:_ITERATOR_DEBUG_LEVEL=0>
        $<$<CONFIG:RelWithDebInfo>:_ITERATOR_DEBUG_LEVEL=0>
        $<$<CONFIG:MinSizeRel>:_ITERATOR_DEBUG_LEVEL=0>
    )
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

# ======================== GoogleTest 配置 ===============================

# 查找 gtest 库
find_library(GTEST_LIB
    NAMES
        gtest
        gtest.lib
        libgtest.a
        libgtest.so
    PATHS "${GTEST_ROOT}/lib"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)

find_library(GTEST_MAIN_LIB
    NAMES
        gtest_main
        gtest_main.lib
        libgtest_main.a
        libgtest_main.so
    PATHS "${GTEST_ROOT}/lib"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)

find_library(GMOCK_LIB
    NAMES
        gmock
        gmock.lib
        libgmock.a
        libgmock.so
    PATHS "${GTEST_ROOT}/lib"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)

find_library(GMOCK_MAIN_LIB
    NAMES
        gmock_main
        gmock_main.lib
        libgmock_main.a
        libgmock_main.so
    PATHS "${GTEST_ROOT}/lib"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)

if(GTEST_LIB AND GTEST_MAIN_LIB AND GMOCK_LIB AND GMOCK_MAIN_LIB)
    # 创建GoogleTest库目标
    add_library(GTest::gtest STATIC IMPORTED)
    set_target_properties(GTest::gtest PROPERTIES
        IMPORTED_LOCATION "${GTEST_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${GTEST_ROOT}/include"
    )

    # 创建GoogleTest主程序库目标
    add_library(GTest::gtest_main STATIC IMPORTED)
    set_target_properties(GTest::gtest_main PROPERTIES
        IMPORTED_LOCATION "${GTEST_MAIN_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${GTEST_ROOT}/include"
        INTERFACE_LINK_LIBRARIES GTest::gtest
    )

    # 创建GoogleMock库目标
    add_library(GTest::gmock STATIC IMPORTED)
    set_target_properties(GTest::gmock PROPERTIES
        IMPORTED_LOCATION "${GMOCK_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${GTEST_ROOT}/include"
        INTERFACE_LINK_LIBRARIES GTest::gtest
    )

    # 创建GoogleMock主程序库目标
    add_library(GTest::gmock_main STATIC IMPORTED)
    set_target_properties(GTest::gmock_main PROPERTIES
        IMPORTED_LOCATION "${GMOCK_MAIN_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${GTEST_ROOT}/include"
        INTERFACE_LINK_LIBRARIES GTest::gmock
    )
    
    # 为GoogleTest目标设置运行时库
    set_target_properties(GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
    )

    message(STATUS "Found GoogleTest libraries")
else()
    message(WARNING "Could not find all GoogleTest libraries")
endif()

# ======================== fmt 配置 ===============================

# 查找 fmt 库
find_library(FMT_LIB
    NAMES 
        fmt
        fmt.lib
        libfmt.a
        libfmt.so
    PATHS "${FMT_DIR}/lib"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)

if(FMT_LIB)
    add_library(fmt::fmt STATIC IMPORTED)
    set_target_properties(fmt::fmt PROPERTIES
        IMPORTED_LOCATION "${FMT_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${FMT_DIR}/include"
    )
    message(STATUS "Found fmt library: ${FMT_LIB}")
else()
    message(WARNING "Could not find fmt library")
endif()

# ======================== protobuf 配置 ===============================
# 查找 protobuf 库
find_library(PROTOBUF_LIB
    NAMES 
        protobuf
        protobuf.lib
    	libprotobuf.lib
        libprotobuf.a
        libprotobuf.so
    PATHS "${PROTOBUF_DIR}/lib"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)

find_library(PROTOC_LIB
    NAMES 
        protoc
        libprotoc.lib
        libprotoc.a
        libprotoc.so
    PATHS "${PROTOBUF_DIR}/lib"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)

if(PROTOBUF_LIB AND PROTOC_LIB)
    # 使用 find_package 但覆盖库位置
    list(APPEND CMAKE_PREFIX_PATH "${PROTOBUF_DIR}/lib/cmake")
    find_package(Protobuf CONFIG)
    
    if(TARGET protobuf::libprotobuf)
        # 覆盖预编译包的库路径
        set_target_properties(protobuf::libprotobuf PROPERTIES
            IMPORTED_LOCATION "${PROTOBUF_LIB}"
            IMPORTED_LOCATION_DEBUG "${PROTOBUF_LIB}"
            IMPORTED_LOCATION_RELEASE "${PROTOBUF_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${PROTOBUF_DIR}/include"
        )
    else()
        # 手动创建目标
        add_library(protobuf::libprotobuf STATIC IMPORTED)
        set_target_properties(protobuf::libprotobuf PROPERTIES
            IMPORTED_LOCATION "${PROTOBUF_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${PROTOBUF_DIR}/include"
        )
    endif()
    
    if(TARGET protobuf::libprotoc)
        set_target_properties(protobuf::libprotoc PROPERTIES
            IMPORTED_LOCATION "${PROTOC_LIB}"
            IMPORTED_LOCATION_DEBUG "${PROTOC_LIB}"
            IMPORTED_LOCATION_RELEASE "${PROTOC_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${PROTOBUF_DIR}/include"
        )
    else()
        add_library(protobuf::libprotoc STATIC IMPORTED)
        set_target_properties(protobuf::libprotoc PROPERTIES
            IMPORTED_LOCATION "${PROTOC_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${PROTOBUF_DIR}/include"
            INTERFACE_LINK_LIBRARIES protobuf::libprotobuf
        )
    endif()
    
    message(STATUS "Found protobuf libraries")
else()
    message(WARNING "Could not find protobuf libraries")
endif()

# ==================== gRPC 配置 ====================
# 查找 gRPC 相关库
find_library(GRPC_LIB
    NAMES 
        grpc
        grpc.lib
        libgrpc.a
        libgrpc.so
    PATHS "${GRPC_DIR}/lib"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)

find_library(GRPC_PP_LIB
    NAMES 
        grpc++
        grpc++.lib
        libgrpc++.a
        libgrpc++.so
    PATHS "${GRPC_DIR}/lib"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)

if(GRPC_LIB AND GRPC_PP_LIB)
    # 尝试使用 find_package
    list(APPEND CMAKE_PREFIX_PATH "${GRPC_DIR}/lib/cmake")
    find_package(gRPC CONFIG QUIET)
    
    if(TARGET gRPC::grpc++)
        # 使用找到的包
        message(STATUS "Using gRPC CMake package")
    else()
        # 手动创建目标
        add_library(gRPC::grpc UNKNOWN IMPORTED)
        set_target_properties(gRPC::grpc PROPERTIES
            IMPORTED_LOCATION "${GRPC_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${GRPC_DIR}/include"
        )
        
        add_library(gRPC::grpc++ UNKNOWN IMPORTED)
        set_target_properties(gRPC::grpc++ PROPERTIES
            IMPORTED_LOCATION "${GRPC_PP_LIB}"
            INTERFACE_INCLUDE_DIRECTORIES "${GRPC_DIR}/include"
            INTERFACE_LINK_LIBRARIES gRPC::grpc
        )
        
        if(TARGET protobuf::libprotobuf)
            set_target_properties(gRPC::grpc++ PROPERTIES
                INTERFACE_LINK_LIBRARIES "gRPC::grpc;protobuf::libprotobuf"
            )
        endif()
    endif()
    
    message(STATUS "Found gRPC libraries")
else()
    message(WARNING "Could not find gRPC libraries")
endif()


# ==================== nlohmann/json 配置 ====================
# 创建nlohmann::json目标（头文件库）
add_library(nlohmann::json INTERFACE IMPORTED)
set_target_properties(nlohmann::json PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${JSON_DIR}/include"
)

# ==================== redis/hiredis 配置 ====================

find_library(HIREDIS_LIB
    NAMES
        hiredis
        hiredis.lib
        libhiredis.a
        libhiredis.so
    PATHS "${REDIS_DIR}/lib"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)

# 查找 Win32_Interop 库（Windows 需要）
if(WIN32)
    find_library(WIN32_INTEROP_LIB
        NAMES
            Win32_Interop
            Win32_Interop.lib
        PATHS "${REDIS_DIR}/lib"
        NO_DEFAULT_PATH
        NO_CMAKE_FIND_ROOT_PATH
    )
endif()

if(HIREDIS_LIB)
    # 创建 hiredis 目标
    add_library(hiredis::hiredis STATIC IMPORTED)
    
    # 设置库路径和头文件路径
    set_target_properties(hiredis::hiredis PROPERTIES
        IMPORTED_LOCATION "${HIREDIS_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${REDIS_DIR}/deps/hiredis"
    )
    
    # Windows 平台需要链接 Win32_Interop
    if(WIN32 AND WIN32_INTEROP_LIB)
        set_target_properties(hiredis::hiredis PROPERTIES
            INTERFACE_LINK_LIBRARIES "${WIN32_INTEROP_LIB}"
        )
    endif()
    
    # 设置运行时库（Windows）
    if(MSVC)
        set_target_properties(hiredis::hiredis PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
        )
    endif()
    
    message(STATUS "Found hiredis library: ${HIREDIS_LIB}")
else()
    message(WARNING "Could not find hiredis library")
endif()



# ==================== 添加子目录 ====================
add_subdirectory(Log)
add_subdirectory(util)
add_subdirectory(net)
if(WIN32)
    add_subdirectory(ZhKeyesIMClient)
endif()
add_subdirectory(ZhkeyesIMServer)
add_subdirectory(GateServer)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()


# ==================== 安装配置 ====================
# 安装可执行文件
if(WIN32)
    install(TARGETS ZhKeyesIMClient ZhkeyesIMServer GateServer
        RUNTIME DESTINATION bin
        CONFIGURATIONS Debug Release
    )
else()
    install(TARGETS ZhkeyesIMServer GateServer
        RUNTIME DESTINATION bin
        CONFIGURATIONS Debug Release
    )
endif()

# 安装动态库
install(TARGETS util net Log
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    CONFIGURATIONS Debug Release
)

# 安装头文件
install(DIRECTORY Log/
    DESTINATION include/Log
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*.hpp"
)

install(DIRECTORY util/
    DESTINATION include/util
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*.hpp"
)

install(DIRECTORY net/
    DESTINATION include/net
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*.hpp"
)

# 安装CMake配置文件
install(EXPORT ZhKeyesIMTargets
    FILE ZhKeyesIMTargets.cmake
    NAMESPACE ZhKeyesIM::
    DESTINATION lib/cmake/ZhKeyesIM
)

# 生成并安装配置文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ZhKeyesIMConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ZhKeyesIMConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ZhKeyesIMConfig.cmake"
    INSTALL_DESTINATION lib/cmake/ZhKeyesIM
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/ZhKeyesIMConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ZhKeyesIMConfigVersion.cmake"
    DESTINATION lib/cmake/ZhKeyesIM
)

