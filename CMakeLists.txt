cmake_minimum_required(VERSION 3.16)
project(ZhKeyesIM VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

set(GTEST_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest")

list(APPEND CMAKE_PREFIX_PATH ${GTEST_ROOT})

find_package(GTest CONFIG REQUIRED)


if(MSVC)  #Microsoft Visual C++
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")   #/W4 启动所有编译警告
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")  #Debug使用多线程调试DLL
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT") #Release使用多线程调试DLL
else()    #GCC/Clang
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

if(EXSITS "${MAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests))
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