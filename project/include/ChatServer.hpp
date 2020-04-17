#ifndef PROJECTKONAMI_CHATSERVER_HPP
#define PROJECTKONAMI_CHATSERVER_HPP

#include <Wt/WServer.h>
#include <mutex>

#include "Auth.hpp"
#include "SessionManager.hpp"

class ChatServer : public Wt::WServer {
public:
    // a reference to client
    class Client {};

    ChatServer(Wt::WServer& server);

    // delete
    ChatServer(const ChatServer&) = delete;
    ChatServer& operator=(const ChatServer&) = delete;



private:
    Wt::WServer& server_;
    std::recursive_mutex mutex_;

    SessionManager sessionManager_;
    Auth           authService_;

};


#endif //PROJECTKONAMI_CHATSERVER_HPP
