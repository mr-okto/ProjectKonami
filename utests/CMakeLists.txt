#project(messenger_tests)
find_package( GTest REQUIRED ON)

set(TEST_SOURCES auth_tests.cpp session_manager_tests.cpp scheduler_tests.cpp thread_pool_tests.cpp)

add_executable(tests main.cpp ${sources} ${TEST_SOURCES})
target_link_libraries(tests gmock gtest GTest::Main)