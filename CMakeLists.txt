cmake_minimum_required(VERSION 3.15.04)
project(chat)
set(CMAKE_CXX_STANDARD 14)
include_directories(include/ /usr/local/include/)

add_library(libwt SHARED IMPORTED GLOBAL)
add_library(libwthttp SHARED IMPORTED GLOBAL)

set_target_properties(libwt PROPERTIES IMPORTED_LOCATION "/usr/local/lib/libwt.dylib")
set_target_properties(libwthttp PROPERTIES IMPORTED_LOCATION "/usr/local/lib/libwthttp.dylib")

add_subdirectory(utests)

set(SOURCES src/main.cpp src/ChatServer.cpp src/ChatApplication.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC libwt LINK_PUBLIC libwthttp)
# ./chat --docroot . --http-address 127.0.0.1 --http-port 8080