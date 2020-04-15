include(GoogleTest)
find_package(GTest REQUIRED)

set (PROJECT_DIR ../project)

include_directories(include ${PROJECT_DIR}/include/ ${GTEST_INCLUDE_DIRS})

set(TEST_SOURCES src/DbConnectorStub.cpp src/TestDb.cpp src/test.cpp)


add_executable(test ${TEST_SOURCES})

target_link_libraries(test gmock gtest pthread)