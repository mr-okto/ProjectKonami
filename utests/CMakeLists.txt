include(GoogleTest)
find_package(GTest REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(include ${PROJECT_SOURCE_DIR}/project/include/ ${GTEST_INCLUDE_DIRS})

file(GLOB UNIT_SOURCES ${PROJECT_SOURCE_DIR}/project/src/*.cpp)
list(FILTER UNIT_SOURCES EXCLUDE REGEX ".main.cpp$")
set(TEST_SOURCES src/TestDb.cpp src/TestMain.cpp)

if (NOT TARGET passwdqc)
add_library(passwdqc STATIC  ${PROJECT_SOURCE_DIR}/project/src/passwdqc/passwdqc_check.c ${PROJECT_SOURCE_DIR}/project/src/passwdqc/passwdqc.h)
endif(NOT TARGET passwdqc)

add_executable(Test ${UNIT_SOURCES} ${TEST_SOURCES})
target_link_libraries(Test gmock gtest pthread wthttp wt wtdbo wtdbopostgres Threads::Threads ${OpenCV_LIBS} passwdqc boost_filesystem)
set_target_properties(Test PROPERTIES COMPILE_FLAGS "-DPOSTGRES")