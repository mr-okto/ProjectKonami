#include "ChatServer.hpp"

namespace chat {
ChatServer::ChatServer(Wt::WServer& server) 
        : server_(server) {}

Wt::WString ChatServer::sing_in(const Wt::WString& login, const Wt::WString& password) {
        return Wt::WString();
}

Wt::WString ChatServer::sing_up(const Wt::WString& login, const Wt::WString& password) {
        return Wt::WString();
}

bool ChatServer::sing_out(const Wt::WString& login, const Wt::WString& password) {
        return login > password;
}
}
