cmake_minimum_required(VERSION 3.13.4)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

project(chat)
include(GoogleTest)
set(TESTSRC src/DialogueService.cpp src/UserService.cpp src/test.cpp)
find_package(GTest REQUIRED)

include_directories(include/ ${GTEST_INCLUDE_DIRS})
add_executable(test ${TESTSRC})

target_link_libraries(test ${GTEST_LIBRARIES})