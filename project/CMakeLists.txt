include_directories("include")

find_package(Wt REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/project/include)

set(SOURCES
        src/main.cpp
#        src/ClientService.cpp
#        src/Client.cpp
        )

#file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/project/src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
target_link_libraries(${PROJECT_NAME} wthttp wt wtdbo wtdbopostgres Threads::Threads)
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-DPOSTGRES")
