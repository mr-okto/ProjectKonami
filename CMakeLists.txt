cmake_minimum_required(VERSION 3.13.4)
project(messenger)

set(CMAKE_CXX_STANDARD 17)

#set(SRCS src/Session.cpp src/SessionManager.cpp src/Auth.cpp src/Scheduler.cpp src/InterruptableSleep.cpp include/InterruptableSleep.hpp src/ThreadPool.cpp include/ThreadPool.hpp)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
include_directories(include)

add_subdirectory(utests)
#add_subdirectory(3rdparty/Scheduler)

add_executable(messenger_project main.cpp ${sources})

#enable_testing()
