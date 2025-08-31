# 测试目录的CMakeLists.txt

# 设置测试源文件
set(UNIT_TEST_SOURCES
    unit/test_http_server.cpp
    unit/test_http_client.cpp
    unit/test_http_parser.cpp
)

set(INTEGRATION_TEST_SOURCES
    integration/test_http_flow.cpp
)

set(TEST_HEADERS
    fixtures/http_test_data.h
)

# 创建单元测试可执行文件
add_executable(unit_tests ${UNIT_TEST_SOURCES} ${TEST_HEADERS})

target_include_directories(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/net
    ${CMAKE_SOURCE_DIR}/net/Http
    ${CMAKE_SOURCE_DIR}/Log
    ${CMAKE_SOURCE_DIR}/util
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/tests/fixtures
)

target_link_libraries(unit_tests
    PRIVATE
    net
    util
    Log
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
)

# 创建集成测试可执行文件
add_executable(integration_tests ${INTEGRATION_TEST_SOURCES} ${TEST_HEADERS})

target_include_directories(integration_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/net
    ${CMAKE_SOURCE_DIR}/net/Http
    ${CMAKE_SOURCE_DIR}/Log
    ${CMAKE_SOURCE_DIR}/util
    ${CMAKE_SOURCE_DIR}/tests
    ${CMAKE_SOURCE_DIR}/tests/fixtures
)

target_link_libraries(integration_tests
    PRIVATE
    net
    util
    Log
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
)

# 设置测试输出目录
set_target_properties(unit_tests integration_tests
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    DEBUG_POSTFIX ""
    RELEASE_POSTFIX ""
)

# 为测试目标设置统一的运行时库配置 - 这是关键修复
if(MSVC)
    set_target_properties(unit_tests integration_tests PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )
    
    # 确保测试目标使用与主项目相同的运行时库设置
    target_compile_options(unit_tests PRIVATE 
        $<$<CONFIG:Debug>:/MDd>
        $<$<CONFIG:Release>:/MD>
        $<$<CONFIG:RelWithDebInfo>:/MD>
        $<$<CONFIG:MinSizeRel>:/MD>
    )
    
    target_compile_options(integration_tests PRIVATE 
        $<$<CONFIG:Debug>:/MDd>
        $<$<CONFIG:Release>:/MD>
        $<$<CONFIG:RelWithDebInfo>:/MD>
        $<$<CONFIG:MinSizeRel>:/MD>
    )

      # 为测试目标设置调试级别
      target_compile_definitions(unit_tests PRIVATE 
      TESTING_BUILD
      $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=2>
      $<$<CONFIG:Release>:_ITERATOR_DEBUG_LEVEL=0>
  )
  
  target_compile_definitions(integration_tests PRIVATE 
      TESTING_BUILD
      $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=2>
      $<$<CONFIG:Release>:_ITERATOR_DEBUG_LEVEL=0>
  )
endif()

# 添加测试到CTest
add_test(NAME UnitTests COMMAND unit_tests)
add_test(NAME IntegrationTests COMMAND integration_tests)

# 设置测试超时时间（秒）
set_tests_properties(UnitTests IntegrationTests PROPERTIES TIMEOUT 300)

# 设置测试工作目录
set_tests_properties(UnitTests IntegrationTests PROPERTIES 
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 为测试设置编译定义
target_compile_definitions(unit_tests PRIVATE
    TESTING_BUILD
    _DEBUG
)

target_compile_definitions(integration_tests PRIVATE
    TESTING_BUILD
    _DEBUG
)

# 设置测试的编译选项
if(MSVC)
    target_compile_options(unit_tests PRIVATE /W4)
    target_compile_options(integration_tests PRIVATE /W4)
else()
    target_compile_options(unit_tests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(integration_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 确保测试在构建后运行
add_custom_command(TARGET unit_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Unit tests built successfully"
)

add_custom_command(TARGET integration_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Integration tests built successfully"
)