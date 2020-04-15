include_directories("include")

set(SOURCES
        src/main.cpp
        src/Server.cpp
        src/Client.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCES})
