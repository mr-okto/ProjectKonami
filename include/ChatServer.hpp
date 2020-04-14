#pragma once
#include <Wt/WServer.h>

class ChatServer {
 public:
    ChatServer(Wt::WServer& server);
 private:
    Wt::WServer& server_;
};
