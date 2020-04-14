set(CMAKE_CXX_STANDARD 17)

file(GLOB sources ${PROJECT_SOURCE_DIR}/project/src/*.cpp)
include_directories(${PROJECT_SOURCE_DIR}/project/include)

add_executable(${PROJECT_NAME} ${sources})
