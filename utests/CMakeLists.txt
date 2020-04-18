include(GoogleTest)
find_package( GTest REQUIRED)


include_directories(${PROJECT_SOURCE_DIR}/project/include/ ${PROJECT_SOURCE_DIR}/utests/include ${GTEST_INCLUDE_DIRS})

file(GLOB TEST_SOURCES src/*.cpp)
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/project/src/*.cpp)
list(FILTER SOURCES EXCLUDE REGEX ".main.cpp$")

add_executable(tests ${SOURCES} ${TEST_SOURCES})
target_link_libraries(tests gmock gtest GTest::Main pthread)
