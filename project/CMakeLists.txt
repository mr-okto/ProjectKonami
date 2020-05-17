find_package(Wt REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${PROJECT_SOURCE_DIR}/project/include)

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/project/src/*.cpp)

if (NOT TARGET passwdqc)
    add_library(passwdqc STATIC ${PROJECT_SOURCE_DIR}/project/src/passwdqc/passwdqc_check.c ${PROJECT_SOURCE_DIR}/project/src/passwdqc/passwdqc.h)
endif(NOT TARGET passwdqc)

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
target_link_libraries(${PROJECT_NAME} wthttp wt wtdbo wtdbopostgres Threads::Threads ${OpenCV_LIBS} passwdqc boost_filesystem)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-DPOSTGRES")
