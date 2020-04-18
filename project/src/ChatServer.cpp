#include <Wt/WServer.h>

#include "ChatServer.hpp"

class UserManagerStub : public UserManager {
public:
    UserModelPtr get_user(const std::string &username) override {
        stubUser.username_ = "login";
        stubUser.pwd_hash_ = "pass";
        stubUser.id_ = 0;

        return &stubUser;
    }

    UserModel stubUser;
};

UserManagerStub userManagerStub;

ChatServer::ChatServer(Wt::WServer& server)
    : server_(server),
      sessionManager_(),
      authService_(userManagerStub, &sessionManager_)
{

}

bool ChatServer::sign_in(const Wt::WString& username, const Wt::WString& password) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

    AuthData data;
    if (authService_.sign_in(username.toUTF8(), password.toUTF8(), std::ref(data))) {
        Session s(data.id);
        sessionManager_.add_session(s);

        return true;
    } else
        return false;

}

bool ChatServer::connect(Client *client, const ChatEventCallback &handle_event) {
    std::unique_lock<std::recursive_mutex> lock(mutex_);

//    if (sessionManager_.has(client)) {
//        return false;
//    }


}

