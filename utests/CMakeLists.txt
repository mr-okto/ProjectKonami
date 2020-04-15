cmake_minimum_required(VERSION 3.13.4)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")

project(chat)
include(GoogleTest)
set(TESTSRC ../project/src/DialogueService.cpp ../project/src/UserService.cpp src/DSTest.cpp src/USTest.cpp src/test.cpp)
find_package(GTest REQUIRED)

include_directories(../project/include/ ${GTEST_INCLUDE_DIRS})
add_executable(test ${TESTSRC})

target_link_libraries(test /usr/local/lib/libgmock.a /usr/local/lib/libgtest.a)
