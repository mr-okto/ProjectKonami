cmake_minimum_required(VERSION 2.8)

project( BlurImage )
find_package( OpenCV REQUIRED )
find_package( Threads )

add_executable( BlurImage blur.cpp blur.hpp)
target_link_libraries( BlurImage ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT})
