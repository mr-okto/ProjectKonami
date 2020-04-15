cmake_minimum_required(VERSION 3.13.4)
project(ProjectKonami)
set(CMAKE_CXX_STANDARD 17)

include_directories("project/include/")
include_directories("/usr/include")

link_directories("/usr/lib")
link_directories("/usr/local/lib")

add_subdirectory(utests)
add_subdirectory(project)
