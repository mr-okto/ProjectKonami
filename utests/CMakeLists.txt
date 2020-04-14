find_package( GTest REQUIRED ON)

#include(${PROJECT_SOURCE_DIR}/utests/include)

set(TEST_SOURCES src/main.cpp src/auth_tests.cpp src/session_manager_tests.cpp src/scheduler_tests.cpp src/thread_pool_tests.cpp)
file(GLOB sources ${PROJECT_SOURCE_DIR}/project/src/*.cpp)
list(FILTER sources EXCLUDE REGEX ".main.cpp$")

add_executable(tests ${sources} ${TEST_SOURCES})
target_link_libraries(tests gmock gtest GTest::Main)
target_include_directories(tests PUBLIC ${PROJECT_SOURCE_DIR}/project/include)