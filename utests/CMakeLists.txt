find_package( GTest REQUIRED ON)
include(GoogleTest)

find_package(GTest REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/project/include/ ${PROJECT_SOURCE_DIR}/utests/include ${GTEST_INCLUDE_DIRS})

set(TEST_SOURCES src/main.cpp src/auth_tests.cpp src/session_manager_tests.cpp src/scheduler_tests.cpp src/thread_pool_tests.cpp src/DSTest.cpp src/USTest.cpp)
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/project/src/*.cpp)
list(FILTER SOURCES EXCLUDE REGEX ".main.cpp$")

add_executable(tests ${SOURCES} ${TEST_SOURCES})
target_link_libraries(tests gmock gtest GTest::Main)
