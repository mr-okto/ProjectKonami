include(GoogleTest)
find_package(GTest REQUIRED)

set (PROJECT_DIR ../project)

include_directories(include ${PROJECT_DIR}/include/ ${GTEST_INCLUDE_DIRS})

set(UNIT_SOURCES ${PROJECT_DIR}/src/Client.cpp ${PROJECT_DIR}/src/ClientService.cpp)

set(TEST_SOURCES src/DbConnectorStub.cpp src/TestDb.cpp src/TestClientService.cpp src/TestClient.cpp src/test.cpp)


add_executable(test ${UNIT_SOURCES} ${TEST_SOURCES})

target_link_libraries(test gmock gtest pthread)