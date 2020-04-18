set(CMAKE_CXX_STANDARD 17)

find_package(Wt REQUIRED)

file(GLOB sources ${PROJECT_SOURCE_DIR}/project/src/*.cpp)
include_directories(${PROJECT_SOURCE_DIR}/project/include)

add_executable(${PROJECT_NAME} ${sources})
#target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
target_link_libraries(${PROJECT_NAME} wthttp wt)
