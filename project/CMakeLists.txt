include_directories("include")

set(SOURCES
        src/main.cpp
        src/DbSession.cpp
        src/UserManager.cpp
        src/MessageManager.cpp
        src/DialogueManager.cpp
        )

add_executable(${PROJECT_NAME} ${SOURCES})
