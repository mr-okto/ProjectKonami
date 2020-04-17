#include "ChatServer.hpp"

class UserManagerStub : public UserManager {
public:
    UserModelPtr get_user(const std::string &username) override { return std::}
};

ChatServer::ChatServer(Wt::WServer& server)
    : server_(server),
      sessionManager_(),
      authService_()
{

}

