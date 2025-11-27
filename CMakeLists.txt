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
set(PROTOBUF_DIR "${THIRD_PARTY_DIR}/grpc")
set(JSON_DIR "${THIRD_PARTY_DIR}/json")
set(REDIS_DIR "${THIRD_PARTY_DIR}/redis")
set(MYSQL_CONNECTOR_DIR "${THIRD_PARTY_DIR}/mysql-connector/Debug")

# list(APPEND CMAKE_PREFIX_PATH "${QT_ROOT}")


# 编译器特定设置 - 修复运行时库不匹配问题
if(MSVC)
    add_compile_options(/utf-8)
    add_compile_definitions(NOMINMAX)
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
    

       # 确保链接器生成 PDB 文件
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG")
    
    # 设置 PDB 输出目录
    set(CMAKE_PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
        
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

function(copy_fmt_dlls_to_target target_name)
    if(WIN32)
        # 根据构建类型选择对应的 DLL
        set(FMT_DLL_DEBUG "${FMT_DIR}/bin/Debug/fmtd.dll")
        set(FMT_DLL_RELEASE "${FMT_DIR}/bin/Release/fmt.dll")
        
        # Debug 配置：拷贝 Debug DLL
        if(EXISTS "${FMT_DLL_DEBUG}")
            add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${FMT_DLL_DEBUG}"
                "$<TARGET_FILE_DIR:${target_name}>/fmtd.dll"
                COMMENT "Copying fmt Debug DLL to ${target_name} output directory"
                CONFIGURATIONS Debug
            )
        endif()
        
        # Release 配置：拷贝 Release DLL
        if(EXISTS "${FMT_DLL_RELEASE}")
            add_custom_command(TARGET ${target_name} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${FMT_DLL_RELEASE}"
                "$<TARGET_FILE_DIR:${target_name}>/fmt.dll"
                COMMENT "Copying fmt Release DLL to ${target_name} output directory"
                CONFIGURATIONS Release RelWithDebInfo MinSizeRel
            )
        endif()
    endif()
endfunction()

# ======================== Abseil, Protobuf, gRPC 配置 ===============================
# 添加 CMake 模块搜索路径
list(APPEND CMAKE_PREFIX_PATH 
    "${GRPC_DIR}/lib/cmake/absl"
    "${GRPC_DIR}/lib/cmake/protobuf"
    "${GRPC_DIR}/lib/cmake/grpc"
    "${GRPC_DIR}/lib/cmake/re2"
    "${GRPC_DIR}/lib/cmake/utf8_range"
)

set(ABSL_PROPAGATE_CXX_STD ON)

# 查找 Abseil（Protobuf 依赖它）
find_package(absl CONFIG REQUIRED)
if(absl_FOUND)
    message(STATUS "Found Abseil via CMake config")
endif()

# 查找 utf8_range（Protobuf 可能依赖它）
find_package(utf8_range CONFIG QUIET)

# 查找 Protobuf（会自动链接 Abseil）
find_package(Protobuf CONFIG REQUIRED)
if(Protobuf_FOUND)
    message(STATUS "Found Protobuf via CMake config")
    message(STATUS "Protobuf version: ${Protobuf_VERSION}")
endif()

# 查找 gRPC（会自动链接 Protobuf 和 Abseil）
find_package(gRPC CONFIG REQUIRED)
if(gRPC_FOUND)
    message(STATUS "Found gRPC via CMake config")
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


# ==================== mysql/connector 配置 ====================
find_library(MYSQL_CONNECTOR_LIB
    NAMES
        mysqlcppconn
        mysqlcppconn.lib
        mysqlcppconn.a
        mysqlcppconn.so
    PATHS "${MYSQL_CONNECTOR_DIR}/lib64/debug/vs14"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
)

if(WIN32)
    find_file(MYSQL_CONNECTOR_DLL
        NAMES
            mysqlcppconn-10-vs14.dll
            mysqlcppconn.dll
        PATHS "${MYSQL_CONNECTOR_DIR}/lib64/debug"
        NO_DEFAULT_PATH
        NO_CMAKE_FIND_ROOT_PATH
    )
endif()


if(MYSQL_CONNECTOR_LIB)

    add_library(mysqlcppconn::mysqlcppconn SHARED IMPORTED)
    
    # 设置库路径和头文件路径
    set_target_properties(mysqlcppconn::mysqlcppconn PROPERTIES
        IMPORTED_LOCATION "${MYSQL_CONNECTOR_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${MYSQL_CONNECTOR_DIR}/include"
    )
    
    if(WIN32)
        if(MYSQL_CONNECTOR_DLL)
            # 找到 DLL，说明是动态库，需要设置 IMPORTED_IMPLIB
            set_target_properties(mysqlcppconn::mysqlcppconn PROPERTIES
                IMPORTED_LOCATION "${MYSQL_CONNECTOR_DLL}"
                IMPORTED_IMPLIB "${MYSQL_CONNECTOR_LIB}"  # .lib 导入库
            )
            message(STATUS "Found MySQL Connector DLL: ${MYSQL_CONNECTOR_DLL}")
        else()
            # 没找到 DLL，可能是静态库，只设置 IMPORTED_LOCATION
            set_target_properties(mysqlcppconn::mysqlcppconn PROPERTIES
                IMPORTED_LOCATION "${MYSQL_CONNECTOR_LIB}"
            )
        endif()
    endif()

    # 设置运行时库（Windows）
    if(MSVC)
        set_target_properties(mysqlcppconn::mysqlcppconn PROPERTIES
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
        )
    endif()
    
    message(STATUS "Found MySQL Connector library: ${MYSQL_CONNECTOR_DIR}")
else()
    message(WARNING "Could not find MySQL Connector library")
endif()

function(copy_mysql_dlls_to_target target_name)
    if(WIN32 AND TARGET mysqlcppconn::mysqlcppconn)
        get_target_property(MYSQL_CONNECTOR_LOCATION mysqlcppconn::mysqlcppconn IMPORTED_LOCATION)
        if(MYSQL_CONNECTOR_LOCATION)
            get_filename_component(MYSQL_CONNECTOR_DIR "${MYSQL_CONNECTOR_LOCATION}" DIRECTORY)
            
            # 查找 DLL 文件
            file(GLOB MYSQL_DLLS
                "${MYSQL_CONNECTOR_DIR}/*.dll"
                "${MYSQL_CONNECTOR_DIR}/../debug/*.dll"
            )
            
            foreach(DLL_FILE ${MYSQL_DLLS})
                get_filename_component(DLL_NAME ${DLL_FILE} NAME)
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${DLL_FILE}"
                    "$<TARGET_FILE_DIR:${target_name}>/${DLL_NAME}"
                    COMMENT "Copying ${DLL_NAME} to ${target_name} output directory"
                )
            endforeach()
        endif()
    endif()
endfunction()

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

