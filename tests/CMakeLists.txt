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

# 创建单元测试可执行文件
add_executable(unit_tests ${UNIT_TEST_SOURCES})
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
add_executable(integration_tests ${INTEGRATION_TEST_SOURCES})
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