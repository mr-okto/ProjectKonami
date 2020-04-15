include_directories("include")

set(SOURCES
        src/cache.cpp
        include/photoManager.hpp)

add_executable(${PROJECT_NAME} ${SOURCES})