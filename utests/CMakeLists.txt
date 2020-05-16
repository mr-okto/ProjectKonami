include(GoogleTest)
find_package(GTest REQUIRED)

include_directories(include ${PROJECT_SOURCE_DIR}/project/include/ ${GTEST_INCLUDE_DIRS})

set(UNIT_SOURCES ${PROJECT_SOURCE_DIR}/project/src/Randomizer.cpp)
set(TEST_SOURCES src/TestDb.cpp src/test.cpp)


add_executable(test ${UNIT_SOURCES} ${TEST_SOURCES})

target_link_libraries(test gmock gtest pthread wthttp wt wtdbo wtdbopostgres Threads::Threads ${OpenCV_LIBS} boost_filesystem)
set_target_properties(test PROPERTIES COMPILE_FLAGS "-DPOSTGRES")