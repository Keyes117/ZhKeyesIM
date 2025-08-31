cmake_minimum_required(VERSION 3.16)
project(ZhKeyesIM VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用测试支持
enable_testing()

# 设置GoogleTest路径
set(GTEST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest")

# 根据构建类型选择正确的库文件路径
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(GTEST_LIB_DIR "${GTEST_ROOT}/lib/Debug")
else()
    set(GTEST_LIB_DIR "${GTEST_ROOT}/lib/Release")
endif()

# 手动创建GoogleTest目标
message(STATUS "Setting up GoogleTest from: ${GTEST_LIB_DIR}")

# 创建GoogleTest库目标
add_library(GTest::gtest STATIC IMPORTED)
set_target_properties(GTest::gtest PROPERTIES
    IMPORTED_LOCATION "${GTEST_LIB_DIR}/gtest.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${GTEST_ROOT}/include"
)

# 创建GoogleTest主程序库目标
add_library(GTest::gtest_main STATIC IMPORTED)
set_target_properties(GTest::gtest_main PROPERTIES
    IMPORTED_LOCATION "${GTEST_LIB_DIR}/gtest_main.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${GTEST_ROOT}/include"
    INTERFACE_LINK_LIBRARIES GTest::gtest
)

# 创建GoogleMock库目标
add_library(GTest::gmock STATIC IMPORTED)
set_target_properties(GTest::gmock PROPERTIES
    IMPORTED_LOCATION "${GTEST_LIB_DIR}/gmock.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${GTEST_ROOT}/include"
    INTERFACE_LINK_LIBRARIES GTest::gtest
)

# 创建GoogleMock主程序库目标
add_library(GTest::gmock_main STATIC IMPORTED)
set_target_properties(GTest::gmock_main PROPERTIES
    IMPORTED_LOCATION "${GTEST_LIB_DIR}/gmock_main.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${GTEST_ROOT}/include"
    INTERFACE_LINK_LIBRARIES GTest::gmock
)

# 编译器特定设置 - 修复运行时库不匹配问题
if(MSVC)
    # 统一设置运行时库 - 这是关键修复
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    
    # 设置编译选项
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    
    # 为所有目标设置统一的运行时库
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MD")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MD")
    
    # 强制所有目标使用相同的运行时库
    set_property(GLOBAL PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    
    # 为GoogleTest目标设置运行时库
    set_target_properties(GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )
        
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


#设置可执行文件的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#设置动态库输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#设置静态库 和 导入库的输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(Log)
add_subdirectory(util)
add_subdirectory(net)
add_subdirectory(ZhKeyesIMClient)
add_subdirectory(ZhkeyesIMServer)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()

# 添加GTest测试


#安装可执行文件
install(TARGETS ZhKeyesIMClient ZhkeyesIMServer
    RUNTIME DESTINATION bin
    CONFIGURATIONS Debug Release
)

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